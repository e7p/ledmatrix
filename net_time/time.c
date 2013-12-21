/* Network Time for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../lib/ethernet.h"
#include "my_ethernet.h"
#include "timelib.h"
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
extern uint32_t uptime; // TODO: put into main-header

void net_time_setup(void) {
  //ledmatrix_setup();
  //ethernet_setup();
  my_ethernet_setup();
  request_time();
}

static const char p_week[] = "SoMoDiMiDoFrSaSo";

uint8_t update_timer = 0;
uint32_t last_uptime = 0;
uint8_t time_tick;
char text[26];

uint8_t net_time_loop(uint8_t mode) {
  time_tick += uptime - last_uptime;
  last_uptime = uptime;
  if(time_tick > 0 || mode == 2) {
    while(time_tick > 0) {
      update_timer++;
      time++;
      if(update_timer == 0) {
        request_time();
      }
      time_tick--;
    }

    if(mode > 0) {
      //Fr, 21.06.2013 - 23:42:00
      struct tm format;
      gmtime_r(time, &format);
      correct_dst(&format);
      sprintf(text, "%c%c, %02d.%02d.%d - %02d:%02d:%02d",
        p_week[2*format.tm_wday],
        p_week[2*format.tm_wday+1],
        format.tm_mday,
        format.tm_mon + 1,
        format.tm_year + 1900,
        format.tm_hour,
        format.tm_min,
        format.tm_sec);

      writeText(text, 0);
    }
  }
  my_ethernet_loop();
  return 0;
}

/*int main() {
  ledmatrix_setup();
  ethernet_setup();
  net_time_setup();
  while(1) {
    net_time_loop(1);
    shiftPixelData();
  }
}*/
