#include <avr/io.h>
#include "my_ethernet.h"
#include "../lib/ethernet.h"
#include <string.h>
#include <stdio.h>
#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>

#define NTP_LEAP_NO_WARNING 0x00
#define NTP_LEAP_61_SECONDS 0x40
#define NTP_LEAP_59_SECONDS 0x80
#define NTP_LEAP_UNKNOWN 0xC0

#define NTP_VERSION_3 0x18
#define NTP_VERSION_4 0x20

#define NTP_SERVER 0x04
#define NTP_CLIENT 0x03

extern uint32_t uptime; // TODO: put into main-header

uint32_t time;
uint32_t time_boot_offset;

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

//uint8_t ip[4] = {176, 221, 43, 3};
uint8_t ip[4] = {192, 168, 1, 1};
// {176, 221,  43,   3} 0.de.pool.ntp.org
// {192, 168,   1, 122} ep-vostro.local
// {176,   9,  44, 144} s7t.de
struct ntp_frame frame;

void my_ethernet_setup(void) {
  _delay_ms(20);

  frame.flag = NTP_LEAP_UNKNOWN | NTP_VERSION_3 | NTP_CLIENT;
  frame.peer_clock_stratum = 0x00;
  frame.peer_polling_intervall = 0x08; // Every 256 Seconds
  frame.peer_clock_precision = 0xfa;
  frame.root_delay = 0x00000100;
  frame.clock_dispersion = 0x00000100;

  unsigned char status;
  do {
    status = GetStatus(0);
    switch(status) {
      case W5100_SKT_SR_CLOSED:
      OpenSocket(0, W5100_SKT_MR_UDP, 123);
      break;

      case W5100_SKT_SR_UDP:
      // Successful
      break;

      default:
      CloseSocket(0);
    }
  } while(status != W5100_SKT_SR_UDP); // Block until opened
  //_delay_ms(20);
}

void request_time(void) {
  UDPOpen(0, ip, 123);
  Send(0, (char*)&frame, sizeof frame);
}

void my_ethernet_loop(void) {
  unsigned int rsize;
  
  if(GetStatus(0) == W5100_SKT_SR_UDP) {
    rsize = ReceivedSize(0); // find out how many bytes
    if(rsize > 0) {
      if(Receive(0, buf, rsize) != W5100_OK) return; // if we had problems, all done
      struct ntp_frame *frame = (struct ntp_frame*)&buf[8];
      TCNT1 = (uint32_t)(31249 * ((frame->transmit_timestamp_f >> 24) & 0xff) / 255);
      time = __builtin_bswap32(frame->transmit_timestamp_i);
      time_boot_offset = time - uptime;
    } else {
      _delay_us(10);
    }
  }
}
