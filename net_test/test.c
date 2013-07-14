#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../lib/ethernet.h"

int main(void) {
  ledmatrix_setup();
  ethernet_setup();
  unsigned int  sockaddr;
  unsigned char mysocket;
  unsigned int  rsize;
  unsigned char ip[] = {192,168,1,122};

  mysocket = 0; // declare the socket number we will use (0-3)

  /*
   *  The main loop.  Control stays in this loop forever, processing any received packets
   *  and sending any requested data.
   */

  while(1) {
    switch(GetStatus(mysocket)) { // based on current status of socket...
      case W5100_SKT_SR_CLOSED: // if socket is closed...
      if(OpenSocket(mysocket, W5100_SKT_MR_UDP, 1234) == mysocket) { // if successful opening a socket...
        //Listen(mysocket);
        _delay_ms(1);
      }
      break;

      case W5100_SKT_SR_UDP: // if socket connection is established...
      rsize = ReceivedSize(mysocket); // find out how many bytes
      if(rsize > 0) {
        if(Receive(mysocket, buf, rsize) != W5100_OK) break; // if we had problems, all done
        clear();
        writeText(buf+8, 0);
        //for(int i = 0; i < 200; i++) {
        //  setRow(i, (buf[i*2] << 8) | buf[i*2+1]);
        //}
        shiftPixelData();

        strcpy_P((char *)buf, PSTR("ledmatrix$ "));
        UDPOpen(mysocket, ip, 1234);
        if(Send(mysocket, buf, strlen((char *)buf)) == W5100_FAIL) break; // just throw out the packet for now
      } else {
        _delay_us(10);
      }
      break;

      case W5100_SKT_SR_FIN_WAIT:
      case W5100_SKT_SR_CLOSING:
      case W5100_SKT_SR_TIME_WAIT:
      case W5100_SKT_SR_CLOSE_WAIT:
      case W5100_SKT_SR_LAST_ACK:
      //CloseSocket(mysocket);
      break;
    }
  }

  return 0;
}
