/* Network Time for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../lib/ethernet.h"
#include "my_ethernet.h"
#include <stdio.h>
#include <string.h>

uint8_t tick;

void setup() {
  ledmatrix_setup();
  ethernet_setup();
  my_ethernet_setup();
  
  // Initialize Timers
  // 16-bit Timer 1 at 0.966s
  TCCR1B |= (1<<CS12) | (1<<WGM12); // Prescaler 256
  OCR1A = 31250-1;

  // Interrupts
  TIMSK1 |= (1<<OCIE1A);
  sei();
  shiftPixelData();
}

void loop() {
  if(tick > 0) {
    //Fr, 21.06.2013 - 23:42:00
    //writeText("Fr, 21.06.2013 - 23:42:00", 8);
    writeText(text, 0);
    sprintf(text, "Tick %d", tick);
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
