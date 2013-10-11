/* Network Twitter for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../lib/ethernet.h"
#include "my_ethernet.h"
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

uint8_t tick = 1, r = 0;
char text[512];

int dot = 18;

void setup() {
  ledmatrix_setup();
  ethernet_setup();
  strcpy(text, "Twitterwall loading...");
  writeText(text, 0);
  shiftPixelData();
  my_ethernet_setup();
  request_twitter();

  // Initialize Timers
  // 16-bit Timer 1 at 1s
  TCCR1B = (1<<CS12) | (1<<WGM12); // Prescaler 256
  OCR1A = 31250-1;

  // Interrupts
  TIMSK1 |= (1<<OCIE1A);
  sei();
}

uint8_t update_timer = 0;
uint16_t i;

void loop() {
  my_ethernet_loop();
  while(tick > 0) {
    //update_timer++;
    /*if(update_timer == 60) {
      request_twitter();
    }*/
    tick--;
  }
  if(r) {
    if(!scrollText(text, &i)) {
      request_twitter();
    } else {
      shiftPixelData();
      i++;
    }
  }
}

ISR(TIMER1_COMPA_vect) {
  // Clock tick
  tick++;
}

int main() {
  setup();
  while(1) loop();
}
