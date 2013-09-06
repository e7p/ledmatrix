/* Marquee (scrolling message) for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "nyan-animation.h"

char devtal[] = "/dev/tal e.V.";
char hackerspace[] = "Hackerspace Wuppertal";
char dickesg[] = " auf dem Dicken G";

uint8_t msg_length = 0;

uint16_t tick = 0;
uint8_t i, j, b;

// Nyan
uint8_t x;

void drawNyan(uint8_t x, uint8_t frame) {
  uint16_t *s = &nyan[frame*31];
  uint16_t *m = &nyan_mask[frame*31];
  for(uint8_t i = 0; i < 31; i++) {
    if(x+i > 231) break;
    if(x+i >= 31) {
      panelData[x+i-31] |= *s;
      panelData[x+i-31] &= ~(~*s & *m);
    }
    s++; m++;
  }
}

uint8_t f;
uint8_t s;

void setup() {
  ledmatrix_setup();
  msg_length = strlen(devtal);
}

void loop() {
  clear();
  if(tick == 0) {
    i = 200;
    b = 0;
    j = 1;

    x = 0;
    f = 0;
    s = 0;
  }
  if(tick < 150) {
    writeText(devtal, i);
    i--;
  } else if(tick < 200) {
    if(b) {
      writeText(devtal, i);
    }
    b = b ^ 1;
  } else if(tick < 300) {
    writeText(devtal, i);
  } else if(tick < 316) {
    //addDoubleRowDown(i, 0xff, 16-j);
    writeTextY(devtal, i, j);
    writeTextY(hackerspace, 16, 32-j);
    //_delay_ms(990);
    j++;
  } else if(tick < 500) {
    writeText(hackerspace, 16);
  } else if(tick < 900) {
    if(x>=31)
      panelData[x-31] = 0;
    if(x < 231) {
      x++;
    } else {
      x = 0;
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
    shiftPixelData();
    _delay_ms(40);
  } else {
    tick = -1;
  }
  shiftPixelData();
  _delay_ms(10);
  tick++;
}

int main() {
  setup();
  while(1) loop();
}
