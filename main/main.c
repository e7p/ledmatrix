#include <string.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "../lib/ledmatrix.h"
#include "../lib/ethernet.h"
#include "../net_time/time.h"

#define TELNET_CLOSED   0
#define TELNET_INACTIVE 1
#define TELNET_SHELL    2
#define TELNET_PROGRAM  3

extern void net_time_setup(void);
extern uint8_t net_time_loop(uint8_t);
extern void net_twitter_setup(void);
extern uint8_t net_twitter_loop(uint8_t);
extern uint8_t nyan_loop(uint8_t);
extern uint8_t augenkrebs_loop(uint8_t);

struct modules {
  const char* name;
  void (*setup)(void);
  uint8_t (*loop)(uint8_t);
};

#define MODULE_COUNT 3

static const struct modules module_list[MODULE_COUNT] = {
  { "net_time", &net_time_setup, &net_time_loop },
  //{ "net_twitter", &net_twitter_setup, &net_twitter_loop },
  { "nyan", NULL, &nyan_loop },
  { "augenkrebs", NULL, &augenkrebs_loop }
};

const prog_char *welcome_message;

#define MAX_MODULES 16

static uint8_t loaded_modules[MAX_MODULES];
static uint8_t loaded_modules_count = 0;
static uint8_t active_module = 0, rot_module = 0;

uint8_t addModule(char* module) {
  for(int i = 0; i < MODULE_COUNT; i++) {
    if(strcmp(module_list[i].name, module) == 0) {
      for(int j = 0; j < MAX_MODULES; j++) {
        if(loaded_modules[j] == i) {
          // module already loaded
          return 3;
        }
      }
      for(int j = 0; j < MAX_MODULES; j++) {
        if(loaded_modules[j] == 255) {
          loaded_modules[j] = i;
          if(module_list[i].setup != NULL) {
            module_list[i].setup();
          }
          loaded_modules_count++;
          // module loaded
          return 0;
        }
      }
      // already MAX_MODULES loaded
      return 1;
    }
  }
  // module not found
  return 2;
}

uint8_t delModule(char* module) {
  for(int i = 0; i < MAX_MODULES; i++) {
    if(strcmp(module_list[loaded_modules[i]].name, module) == 0) {
      // module unloaded
      loaded_modules[i] = 255;
      loaded_modules_count--;
      return 0;
    }
  }
  // module not found
  return 2;
}

void setup() {
  welcome_message = PSTR("LED Matrix v0.1 - Built " __DATE__ " " __TIME__ "\r\n");
  
  ledmatrix_setup();
  ethernet_setup();
  
  for(int i = 0; i < MAX_MODULES; i++) {
    loaded_modules[i] = 255;
  }
}

uint8_t telnet_status = 0;
uint8_t buf[256];
uint8_t tmp[256];

void loop() {
  /* Telnet Loop */
  uint8_t status = GetStatus(3);
  switch(status) {
    case W5100_SKT_SR_CLOSED:
    telnet_status = TELNET_CLOSED;
    OpenSocket(3, W5100_SKT_MR_TCP, 23);
    break;
    
    case W5100_SKT_SR_INIT:
    Listen(3);
    break;

    case W5100_SKT_SR_ESTABLISHED:
    if(telnet_status == TELNET_CLOSED) {
      strcpy_P((char *)buf, welcome_message);
      Send(3, buf, strlen((char *)buf));
      telnet_status = TELNET_INACTIVE;
    }
    break;

    case W5100_SKT_SR_FIN_WAIT:
    case W5100_SKT_SR_CLOSING:
    case W5100_SKT_SR_TIME_WAIT:
    case W5100_SKT_SR_CLOSE_WAIT:
    case W5100_SKT_SR_LAST_ACK:
    CloseSocket(3);
    telnet_status = TELNET_CLOSED;
    break;
  }
  unsigned int rsize;
  switch(telnet_status) {
    case TELNET_INACTIVE:
    Send(3, "$ ", 2);
    telnet_status = TELNET_SHELL;
    break;
    
    case TELNET_SHELL:
    // Check if data received
    rsize = ReceivedSize(3); // find out how many bytes
    if(rsize > 0) {
      if(Receive(3, buf, rsize) != W5100_OK) break;
      if(buf[0] & 0x80) break;
      char* arg = strtok(buf, " \r\n");
      if(strcmp(arg, "exit") == 0) {
        DisconnectSocket(3);
      } else if(strcmp(arg, "actmod") == 0) {
        arg = strtok(NULL, " \r\n");
        for(int i = 0; i < MAX_MODULES; i++) {
          if(loaded_modules[i] < 255 &&
            strcmp(module_list[loaded_modules[i]].name, arg) == 0) {
            active_module = loaded_modules[i];
            strcpy(tmp, arg);
            sprintf(buf, "Active Module set to: %s(%d)\r\n", tmp, active_module);
            SendString(3, buf);
            break;
          }
        }
      } else if(strcmp(arg, "modprobe") == 0) {
        arg = strtok(NULL, " \r\n");
        strcpy(tmp, arg);
        sprintf(buf, "Module %s loaded, return value: %d\r\n", tmp, addModule(tmp));
        SendString(3, buf);
      } else if(strcmp(arg, "lsmod") == 0) {
        tmp[0] = '\0';
        for(int i = 0; i < MAX_MODULES; i++) {
          if(loaded_modules[i] < 255) {
            strcat(tmp, module_list[loaded_modules[i]].name);
            strcat(tmp, ", ");
          }
        }
        tmp[strlen(tmp)-2] = '\0';
        sprintf(buf, "Modules: %s\r\n", tmp);
        SendString(3, buf);
      } else if(strcmp(arg, "rmmod") == 0) {
        arg = strtok(NULL, " \r\n");
        strcpy(tmp, arg);
        sprintf(buf, "Module %s unloaded, return value: %d\r\n", tmp, delModule(tmp));
        SendString(3, buf);
      } else if(strcmp(arg, "getmod") == 0) {
        sprintf(buf, "Rotated module: %d, Current module: %s(%d)\r\n", rot_module, module_list[active_module].name, active_module);
        SendString(3, buf);
      } else if(strcmp(arg, "help") == 0 || strcmp(arg, "?") == 0) {
        SendString(3, "Defined functions:\n  ?, actmod, exit, getmod, help, lsmod, modprobe, rmmod\r\n");
      } else {
        SendString(3, "Unknown command\r\n");
      }
      telnet_status = TELNET_INACTIVE;
    }
    break;
    
    case TELNET_PROGRAM:
    // Do nothing, program is running
    break;
    
    case TELNET_CLOSED:
    // Do nothing, telnet is closed
    break;
  }

  /* Module Loop functions */
  for(int i = 0; i < MAX_MODULES; i++) {
    if(loaded_modules[i] < 255) {
      if(loaded_modules[i] == active_module) {
        if(module_list[active_module].loop(1) > 0) {
          // rotate active module
          if(rot_module == active_module && loaded_modules_count > 1) {
            do {
              rot_module++;
              if(rot_module == MAX_MODULES) rot_module = 0;
            } while(loaded_modules[rot_module] == 255);
          }
          active_module = loaded_modules[rot_module];
        }
      } else {
        if(module_list[loaded_modules[i]].loop(0) == 255) {
          // set active module to this
          active_module = loaded_modules[i];
        }
      }
    }
  }
  
  //TEMP:
  setPixel(0,0,telnet_status == TELNET_CLOSED);
  setPixel(0,1,telnet_status == TELNET_INACTIVE);
  setPixel(0,2,telnet_status == TELNET_SHELL);
  setPixel(0,3,telnet_status == TELNET_PROGRAM);
  
  shiftPixelData();
}

int main() {
  setup();
  while(1) loop();
}
