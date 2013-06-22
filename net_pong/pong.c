/* Network Pong for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../lib/ethernet.h"
#include "my_ethernet.h"

uint8_t gameTick;

void setup() {
  ledmatrix_setup();
  ethernet_setup();
  
  // Initialize Timers
  // 16-bit Timer 1 at ?
  TCCR1B |= (1<<CS00); // Prescaler 1

  // Interrupts
  TIMSK1 |= (1<<TOIE1);
  sei();
}

void loop() {
  if(gameTick > 64) {
    //foo
    writeText("Connect to 192.168.1.12 555 via telnet", 8);
    shiftPixelData();
    gameTick-=64; // 16=normal
  }
  my_ethernet_loop();
}

ISR(TIMER1_OVF_vect) {
  // Game timer
  gameTick++;
}

int main() {
  setup();
  while(1) loop();
}
