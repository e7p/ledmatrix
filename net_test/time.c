#include <avr/io.h>
#include <string.h>
#include <stdio.h>
#define F_CPU 8000000
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../lib/ethernet.h"


#define  MAX_BUF					512			/* largest buffer we can read from chip */

#define  HTTP_PORT					1234			/* TCP port for HTTP */


int  main(void)
{
  ledmatrix_setup();
  ethernet_setup();
	unsigned int					sockaddr;
	unsigned char					mysocket;
	unsigned int					rsize;


	mysocket = 0;											// magic number! declare the socket number we will use (0-3)
	sockaddr = W5100_SKT_BASE(mysocket);					// calc address of W5100 register set for this socket
 
/*
 *  The main loop.  Control stays in this loop forever, processing any received packets
 *  and sending any requested data.
 */
  //OpenSocket(mysocket, W5100_SKT_MR_UDP, HTTP_PORT);
 
	while  (1)
	{
    
		switch  (W51_read(sockaddr+W5100_SR_OFFSET))		// based on current status of socket...
		{
			case  W5100_SKT_SR_CLOSED:						// if socket is closed...
			if (OpenSocket(mysocket, W5100_SKT_MR_UDP, HTTP_PORT) == mysocket)		// if successful opening a socket...
			{
				//Listen(mysocket);
				_delay_ms(1);
			}
			break;

			case  W5100_SKT_SR_UDP:					// if socket connection is established...
			//else											// no data yet...
			{
        
        rsize = ReceivedSize(mysocket);					// find out how many bytes
        if (rsize > 0)
        {
          if (Receive(mysocket, buf, rsize) != W5100_OK)  break;	// if we had problems, all done
          clear();
          writeText(buf+8, 0);
          //for(int i = 0; i < 200; i++) {
          //  setRow(i, (buf[i*2] << 8) | buf[i*2+1]);
          //}
          shiftPixelData();
  /*
   *  Add code here to process the payload from the packet.
   *
   *  For now, we just ignore the payload and send a canned HTML page so the client at least
   *  knows we are alive.
   */
          /*strcpy_P((char *)buf, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
          strcat_P((char *)buf, PSTR("<html>\r\n<body>\r\n"));
          strcat_P((char *)buf, PSTR("<title>Karl's W5100 web server (ATmega644p)</title>\r\n"));
          strcat_P((char *)buf, PSTR("<h2>Karl's ATmega644p web server using Wiznet W5100 chip</h2>\r\n"));
          strcat_P((char *)buf, PSTR("<br /><hr>\r\n"));
          if (Send(mysocket, buf, strlen((char *)buf)) == W5100_FAIL)  break;		// just throw out the packet for now
  */
          strcpy_P((char *)buf, PSTR("ledmatrix$ "));
          UDPOpen(mysocket, 192,168,1,122, 1234);
          if (Send(mysocket, buf, strlen((char *)buf)) == W5100_FAIL)  break;		// just throw out the packet for now* /
          //DisconnectSocket(mysocket);
        } else {
          _delay_us(10);
        }
			}
			break;//* /

			case  W5100_SKT_SR_FIN_WAIT:
			case  W5100_SKT_SR_CLOSING:
			case  W5100_SKT_SR_TIME_WAIT:
			case  W5100_SKT_SR_CLOSE_WAIT:
			case  W5100_SKT_SR_LAST_ACK:
			//CloseSocket(mysocket);
			break;
		}

	}

	return  0;
}
