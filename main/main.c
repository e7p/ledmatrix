#include <string.h>
#include <avr/pgmspace.h>
#include "../lib/ledmatrix.h"
#include "../lib/ethernet.h"
#include "../net_time/time.h"

#define TELNET_CLOSED   0
#define TELNET_INACTIVE 1
#define TELNET_SHELL    2
#define TELNET_PROGRAM  3

const prog_char *welcome_message;

void setup() {
  welcome_message = PSTR("LED Matrix v0.1 - Built " __DATE__ " " __TIME__ "\r\n");
  
  ledmatrix_setup();
  ethernet_setup();

  /* Module Setup functions */
  setup_net_time();
}

uint8_t telnet_status = 0;
uint8_t buf[512];

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
      char* arg = strtok(buf, " \r\n");
      if(strcmp(arg, "exit") == 0) {
        DisconnectSocket(3);
      } else if(strcmp(arg, "help") == 0 || strcmp(arg, "?") == 0) {
        SendString(3, "Defined functions:\n  ?, exit, help\n");
      } else {
        SendString(3, "Unknown command\n");
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
  loop_net_time();
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
