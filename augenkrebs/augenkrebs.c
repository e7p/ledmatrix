/* Augenkrebs for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "../lib/ledmatrix.h"

uint8_t xy = 0;

uint8_t augenkrebs_loop(uint8_t mode) {
  if(xy == 0) {
    clear();
    xy = 1;
  } else {
    for(uint16_t i = 0; i < PANELDATA_SIZE; i++) {
      panelData[i] = 0xffff;
    }
    xy = 0;
  }
  _delay_ms(50);
  return 0;
}

/*int main() {
  ledmatrix_setup();
  while(1) {
    nyan_loop(1);
    shiftPixelData();
  }
}*/
