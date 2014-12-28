#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "../lib/ledmatrix.h"
#include "../lib/ethernet.h"
#include "../net_time/time.h"

// Foo
#include "../net_time/timelib.h"
#include "../net_time/my_ethernet.h"

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
extern void message_setup(void);
extern uint8_t message_loop(uint8_t);
extern void message_unload(void);
extern void hotnews_setup(void);
extern uint8_t hotnews_loop(uint8_t);
extern void hotnews_unload(void);
extern uint8_t langertisch_loop(uint8_t);
extern uint8_t line_animation_loop(uint8_t);

struct modules {
  const char* name;
  void (*setup)(void);
  uint8_t (*loop)(uint8_t);
  void (*unload)(void);
};

#define MODULE_COUNT 7

static const struct modules module_list[MODULE_COUNT] = {
  { "net_time", &net_time_setup, &net_time_loop, NULL },
  //{ "net_twitter", &net_twitter_setup, &net_twitter_loop, NULL },
  { "nyan", NULL, &nyan_loop, NULL },
  { "augenkrebs", NULL, &augenkrebs_loop, NULL },
  { "message", &message_setup, &message_loop, &message_unload },
  { "hotnews", &hotnews_setup, &hotnews_loop, &hotnews_unload },
  { "langertisch", NULL, &langertisch_loop, NULL },
  { "line_animation", NULL, &line_animation_loop, NULL },
};

struct command {
  const char* name;
  void (*run)(char*);
};

const char welcome_message[] PROGMEM = "LED Matrix v0.1 - Built " __DATE__ " " __TIME__ "\r\n";

#define MAX_MODULES 16
#define MAX_COMMANDS 4

static uint8_t loaded_modules[MAX_MODULES];
static struct command* commands[MAX_COMMANDS];
static uint8_t loaded_modules_count = 0;
static uint8_t active_module = 0, rot_module = 0, last_module = 0;

static uint8_t tick = 0;
uint32_t uptime = 0;

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

uint8_t addCommand(const char* name, void (*callback)(char*)) {
  for(int i = 0; i < MAX_COMMANDS; i++) {
    if(commands[i] == 0)
      continue;
    if(strcmp(commands[i]->name, name) == 0)
      return 0;
  }
  for(int i = 0; i < MAX_COMMANDS; i++) {
    if(commands[i] == 0) {
      struct command *cmd = malloc(sizeof(struct command));
      cmd->name = name;
      cmd->run = callback;
      commands[i] = cmd;
      return 1;
    }
  }
}

uint8_t delModule(char* module) {
  for(int i = 0; i < MAX_MODULES; i++) {
    if(strcmp(module_list[loaded_modules[i]].name, module) == 0) {
      // module unloaded
      if(module_list[loaded_modules[i]].unload != NULL) {
        module_list[loaded_modules[i]].unload();
      }
      loaded_modules[i] = 255;
      loaded_modules_count--;
      return 0;
    }
  }
  // module not found
  return 2;
}

uint8_t delCommand(const char* name) {
  for(int i = 0; i < MAX_COMMANDS; i++) {
    if(commands[i] == 0)
      continue;
    if(strcmp(commands[i]->name, name) == 0) {
      free(commands[i]);
      commands[i] = 0;
      return 1;
    }
  }
  return 0;
}

void setup() {
  ledmatrix_setup();
  ethernet_setup();
  
  for(int i = 0; i < MAX_MODULES; i++) {
    loaded_modules[i] = 255;
  }
  
  // Initialize Timers
  // 16-bit Timer 1 at 1s
  TCCR1B = (1<<CS12) | (1<<WGM12); // Prescaler 256
  OCR1A = 31250-1;
  
  // Interrupts
  TIMSK1 |= (1<<OCIE1A);
  sei();

  // TEMP static bootloader
  addModule("net_time");
  //addModule("hotnews");
}

uint8_t telnet_status = 0;
uint8_t buf[256];
uint8_t tmp[256];

void loop() {
  /* Clock */
  while(tick > 0) {
    tick--;
    uptime++;
  }
  
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
    //break;
    
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
            sprintf(buf, "Active Module set to: %s\r\n", tmp);
            SendString(3, buf);
            break;
          }
        }
      } else if(strcmp(arg, "modprobe") == 0) {
        arg = strtok(NULL, " \r\n");
        strcpy(tmp, arg);
        if(addModule(tmp) == 0) {
          sprintf(buf, "Module %s loaded\r\n", tmp);
        }
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
        if(delModule(tmp) == 0) {
          sprintf(buf, "Module %s unloaded\r\n", tmp);
        }
        SendString(3, buf);
      } else if(strcmp(arg, "getmod") == 0) {
        sprintf(buf, "Rotated module: %d, Current module: %s(%d)\r\n", rot_module, module_list[active_module].name, active_module);
        SendString(3, buf);
      } else if(strcmp(arg, "uptime") == 0) {
        sprintf(buf, "Uptime: %d seconds\r\n", uptime);
        SendString(3, buf);
      } else if(strcmp(arg, "help") == 0 || strcmp(arg, "?") == 0) {
        SendString(3, "Defined functions:\r\n  ?, actmod, exit, getmod, help, lsmod, modprobe, rmmod, uptime\r\n");
        strcpy(tmp, "Module commands:\r\n  ");
        for(int i = 0; i < MAX_COMMANDS; i++) {
          if(commands[i] != 0) {
            strcat(tmp, commands[i]->name);
            strcat(tmp, ", ");
          }
        }
        uint8_t c = strlen(tmp);
        tmp[c-2] = '\r';
        tmp[c-1] = '\n';
        SendString(3, tmp);
      } else {
        for(int i = 0; i < MAX_COMMANDS; i++) {
          if(commands[i] != 0 && strcmp(commands[i]->name, arg) == 0) {
            commands[i]->run(strtok(buf+strlen(arg)+1, "\r\n"));
            goto finish;
          }
        }
        SendString(3, "Unknown command\r\n");
      }
finish: telnet_status = TELNET_INACTIVE;
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
  last_module = active_module;
  for(int i = 0; i < MAX_MODULES; i++) {
    if(loaded_modules[i] < 255) {
      if(loaded_modules[i] == active_module) {
        if(module_list[active_module].loop(last_module == active_module ? 2 : 1) > 0) {
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

ISR(TIMER1_COMPA_vect) {
  // Clock tick
  tick++;
}

int main() {
  setup();
  while(1) loop();
}
