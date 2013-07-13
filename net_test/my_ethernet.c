#include <avr/io.h>
#include "my_ethernet.h"
#include "../lib/ethernet.h"
#include <string.h>
#include <stdio.h>
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

#define FLAG_LEAP_NO_WARNING 0x00
#define FLAG_LEAP_61_SECONDS 0x40
#define FLAG_LEAP_59_SECONDS 0x80
#define FLAG_LEAP_UNKNOWN 0xC0

#define FLAG_VERSION_3 0x18
#define FLAG_VERSION_4 0x20

#define FLAG_SERVER 0x04
#define FLAG_CLIENT 0x03

char text[25];

struct ntp_frame {
  uint8_t flag;
  uint8_t peer_clock_stratum;
  uint8_t peer_polling_intervall;
  uint8_t peer_clock_precision;
  uint32_t root_delay;
  uint32_t clock_dispersion;
  uint8_t reference_clock_id[4];
  uint32_t reference_clock_update_i;
  uint32_t reference_clock_update_f;
  uint32_t originate_timestamp_i;
  uint32_t originate_timestamp_f;
  uint32_t receive_timestamp_i;
  uint32_t receive_timestamp_f;
  uint32_t transmit_timestamp_i;
  uint32_t transmit_timestamp_f;
};

  uint8_t ip[4] = {192, 168, 1, 149};
  
void my_ethernet_setup(void) {
  _delay_ms(20);
  OpenSocket(0, W5100_SKT_MR_UDP, 123);
  struct ntp_frame frame;
  frame.flag = FLAG_LEAP_UNKNOWN | FLAG_VERSION_3 | FLAG_CLIENT;
  frame.peer_clock_stratum = 0x00;
  frame.peer_polling_intervall = 0x03;
  frame.peer_clock_precision = 0xfa;
  frame.root_delay = 0x00010000;
  frame.clock_dispersion = 0x00010000;
  //Connect(1, W5100_SKT_MR_TCP, ip, 7);
  OpenSocket(1, W5100_SKT_MR_TCP, 555);
  
  UDPSend(0, ip, 123, (char*)&frame, 48);
  // {192, 168,   1, 149} ep-vostro.local
  // {176,   9,  44, 144} s7t.de
}


            char *hello = "This is LED-Matrix!\r\n";

void my_ethernet_loop(void) {
  unsigned int rsize;
  rsize = ReceivedSize(0); // find out how many bytes
  if(rsize > 0) {
    if(Receive(0, buf, rsize) != W5100_OK) return; // if we had problems, all done
    struct ntp_frame *frame = (struct ntp_frame*)&buf[8];
    sprintf(text, "Zeit: %8x%8x", (*frame).receive_timestamp_i, (*frame).receive_timestamp_f);
  } else {
    _delay_us(10);
  }
  
  #define SOCKADDR W5100_SKT_BASE(1)
  switch  (W51_read(SOCKADDR+W5100_SR_OFFSET))        // based on current status of socket...
        {
            case  W5100_SKT_SR_CLOSED:                        // if socket is closed...
            _delay_ms(1);
            //Connect(1, W5100_SKT_MR_TCP, ip, 555);
            break;

            case  W5100_SKT_SR_ESTABLISHED:                    // if socket connection is established...
                _delay_us(10);
                if (Send(1, hello, strlen(hello)) == W5100_FAIL)  break;        // just throw out the packet for now
                DisconnectSocket(1);
            break;

            case  W5100_SKT_SR_FIN_WAIT:
            case  W5100_SKT_SR_CLOSING:
            case  W5100_SKT_SR_TIME_WAIT:
            case  W5100_SKT_SR_CLOSE_WAIT:
            case  W5100_SKT_SR_LAST_ACK:
            CloseSocket(1);
            break;
        }
  
}
