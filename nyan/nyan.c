/* Nyan cat for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "../lib/ledmatrix.h"
#include "nyan-animation.h"

uint8_t x = 0;

void drawNyan(uint8_t x, uint8_t frame) {
  uint16_t *s = &nyan[frame*31];
  uint16_t *m = &nyan_mask[frame*31];
  for(uint8_t i = 0; i < 31; i++) {
    if(x+i > PANELDATA_SIZE + 31) break;
    if(x+i >= 31) {
      panelData[x+i-31] |= *s;
      panelData[x+i-31] &= ~(~*s & *m);
    }
    s++; m++;
  }
}

uint8_t f = 0;
uint8_t s = 0;

uint8_t nyan_loop(uint8_t mode) {
  if(mode == 0) return 0;
  if(x>=31)
    panelData[x-31] = 0;
  if(x < PANELDATA_SIZE + 31) {
    x++;
  } else {
    x = 0;
    return 1;
  }
  clear();
  if(x >= 24) {
    for(uint8_t i = 0; i < x - 24; i++) {
      if((i/5)%2 == s/2) {
        panelData[i] = 0x9112;
      } else {
        panelData[i] = 0x4889;
      }
    }
  }
  s++;
  if(s==4) s = 0;
  drawNyan(x, f++);
  if(f == 6) f = 0;
  _delay_ms(60);
  return 0;
}

/*int main() {
  ledmatrix_setup();
  while(1) {
    nyan_loop(1);
    shiftPixelData();
  }
}*/
