/* Nyan cat for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "nyan-animation.h"

#define DATA_LOW PC4
#define DATA_HIGH PC2
#define OE PC3
#define SRCK PC6
#define RCK PC7

#define PANELS 5
#define PANELDATA_SIZE (40*PANELS)

uint16_t panelData[PANELDATA_SIZE];

void clear() {
  for(uint16_t i = 0; i < PANELDATA_SIZE; i++) {
    panelData[i] = 0;
  }
}

void setPixel(uint16_t x, uint8_t y, uint8_t value) {
  if (value)
    panelData[x] |= 1 << y;
  else
    panelData[x] &= ~(1 << y);
}

void setRow(uint8_t x, uint8_t y, uint8_t value) {
  // TODO: Fixme
  panelData[2*x+y] = value;
}

uint16_t getPixel(int x, int y) {
  return panelData[x] & (1 << y);
}

void screen_off() {
  PORTC &= ~(1<<OE);
}

void screen_on() {
  PORTC |= (1<<OE);
}

void shiftPixelData() {
  PORTC &= ~(1<<RCK);
  for(int i = 0; i < PANELDATA_SIZE; i++) {
    uint8_t value_high = panelData[i] >> 8;
    uint8_t value_low = panelData[i];
    for(int j = 0; j < 8; j++) {
      uint8_t portc = ((value_low&1)<<DATA_LOW)|((value_high&1)<<DATA_HIGH)|(1<<OE);
      //PORTC &= ~((1<<DATA_LOW)|(1<<DATA_HIGH));
      //PORTC |= ((value_low&1)<<DATA_LOW)|((value_high&1)<<DATA_HIGH);
      value_low = value_low >> 1;
      value_high = value_high >> 1;
      PORTC = portc;
      _delay_us(1);
      portc |= (1<<SRCK);
      PORTC = portc;
      //_delay_us(5);
    }
  }
  PORTC |= (1<<RCK);
  //_delay_us(2);
  //_delay_us(10);
}

void setup() {
  DDRC |= (1<<DATA_LOW)|(1<<DATA_HIGH)|(1<<OE)|(1<<SRCK)|(1<<RCK);
  screen_on();
}

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

void loop() {
  if(x>=31)
    panelData[x-31] = 0;
  if(x < PANELDATA_SIZE + 31) {
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
  _delay_ms(80);
}

int main() {
  setup();
  while(1) loop();
}
