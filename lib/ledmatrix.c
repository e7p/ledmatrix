#ifndef F_CPU
#define F_CPU 8000000UL
#endif
#include <util/delay.h>
#include "ledmatrix.h"

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

void setDoubleRow(uint8_t x, uint8_t value) {
  panelData[x] = 0;
  for(int i = 0; i < 8; i++) {
    panelData[x] |= ((value&1)*3)<<i*2;
    value = value >> 1;
  }
}

void setRow(uint8_t x, uint16_t value) {
  panelData[x] = value;
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
      value_low = value_low >> 1;
      value_high = value_high >> 1;
      PORTC = portc;
      _delay_us(1);
      portc |= (1<<SRCK);
      PORTC = portc;
    }
  }
  PORTC |= (1<<RCK);
}

void ledmatrix_setup() {
  DDRC |= (1<<DATA_LOW)|(1<<DATA_HIGH)|(1<<OE)|(1<<SRCK)|(1<<RCK);
  
  screen_on();
}
