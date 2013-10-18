/* Network Time for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../lib/ethernet.h"
#include "my_ethernet.h"
#include "timelib.h"
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>

uint8_t tick = 1;

void net_time_setup(void) {
  //ledmatrix_setup();
  //ethernet_setup();
  my_ethernet_setup();
  request_time();

  // Initialize Timers
  // 16-bit Timer 1 at 1s
  TCCR1B = (1<<CS12) | (1<<WGM12); // Prescaler 256
  OCR1A = 31250-1;

  // Interrupts
  TIMSK1 |= (1<<OCIE1A);
  sei();
}

static const char p_week[] = "SoMoDiMiDoFrSaSo";

uint8_t update_timer = 0;
char text[26];

uint8_t net_time_loop(uint8_t mode) {
  if(tick > 0) {
    while(tick > 0) {
      update_timer++;
      time++;
      if(update_timer == 0) {
        request_time();
      }
      tick--;
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

ISR(TIMER1_COMPA_vect) {
  // Clock tick
  tick++;
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
