#include <avr/io.h>
#include <avr/pgmspace.h>
#include "my_ethernet.h"
#include "../lib/ethernet.h"
#include <string.h>
#include <stdio.h>
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

extern char text[512];
extern uint8_t r;

uint8_t ip[4] = { 80,  86, 174,  42};
// { 80,  86, 174,  42} devtal.de
// {192, 168,   1, 192} ep-vostro.local

uint8_t buf[512];

void my_ethernet_setup(void) {
  _delay_ms(50);
  request_twitter();
}

void request_twitter(void) {
  unsigned char status;
  r = 0;
  do {
    status = GetStatus(1);
    switch(status) {
      case W5100_SKT_SR_CLOSED:
      OpenSocket(1, W5100_SKT_MR_TCP, 49152);
      break;
      
      case W5100_SKT_SR_INIT:
      Connect(1, W5100_SKT_MR_TCP, ip, 80);
      break;

      case W5100_SKT_SR_ESTABLISHED:
      strcpy_P((char *)buf, PSTR("GET /ledmatrix/twitter.php HTTP/1.1\r\nHost: www.devtal.de\r\nConnection: close\r\n\r\n"));
      if (Send(1, buf, strlen((char *)buf)) == W5100_FAIL)  break;
      break;

      case W5100_SKT_SR_FIN_WAIT:
      case W5100_SKT_SR_CLOSING:
      case W5100_SKT_SR_TIME_WAIT:
      case W5100_SKT_SR_CLOSE_WAIT:
      case W5100_SKT_SR_LAST_ACK:
      CloseSocket(1);
      break;
    }
  } while(status != W5100_SKT_SR_ESTABLISHED);
}

void my_ethernet_loop(void) {
  unsigned int rsize;
  
  if(GetStatus(1) != W5100_SKT_SR_CLOSED) {
    rsize = ReceivedSize(1); // find out how many bytes
    if(rsize > 0) {
      if(Receive(1, buf, rsize) != W5100_OK) return;
      DisconnectSocket(1);
      char *pos = strstr(buf, "\r\n\r\n");
      if(pos == 0) return;
      pos = strstr(pos + 4, "\r\n");
      if(pos == 0) return;
      // ID = pos+2
      pos = strchr(pos + 2, '\n');
      if(pos == 0) return;
      strncpy(text, pos + 1, rsize - (pos - (char*)buf + 1));
      *(strchr(text, '\r')) = 0;
      r = 1;
      CloseSocket(1);
    } else {
      _delay_us(10);
    }
  }
}
