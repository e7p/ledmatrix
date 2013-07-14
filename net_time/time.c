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

void setup() {
  ledmatrix_setup();
  ethernet_setup(); 
  my_ethernet_setup();

  // Initialize Timers
  // 16-bit Timer 1 at 1s
  TCCR1B = (1<<CS12) | (1<<WGM12); // Prescaler 256
  OCR1A = 31250-1;

  // Interrupts
  TIMSK1 |= (1<<OCIE1A);
  sei();
}

static const char p_week[] PROGMEM = "SoMoDiMiDoFrSaSo";

void loop() {
  if(tick > 0) {
    time += tick;

    //Fr, 21.06.2013 - 23:42:00
    struct tm format;
    gmtime_r(time, &format);
    correct_dst(&format);
    char wday[3];
    wday[0] = pgm_read_byte(&p_week[2*format.tm_wday]);
    wday[1] = pgm_read_byte(&p_week[2*format.tm_wday+1]);
    char text[27];
    sprintf(text, PSTR("%s, %02d.%02d.%d - %02d:%02d:%02d"),
      wday,
      format.tm_mday,
      format.tm_mon + 1,
      format.tm_year + 1900,
      format.tm_hour,
      format.tm_min,
      format.tm_sec);

    writeText(text, 8);

    tick = 0;
  }
  shiftPixelData();
  my_ethernet_loop();
}

ISR(TIMER1_COMPA_vect) {
  // Clock tick
  tick++;
}

int main() {
  setup();
  while(1) loop();
}
