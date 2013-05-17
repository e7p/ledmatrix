/* NFT
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "image.h"

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

void drawImage(uint8_t n, uint8_t x) {
 //uint16_t *s = &image[n];
  for(uint8_t i = 0; i < 70; i++) {
    setRow(i+x, image[n][i]); //*(s++));
  }
}

uint8_t p = 0, s = 0;
void loop() {
  clear();
  s = 0;
  switch(p) {
    case 12: drawImage(1, 0);
    case 11: drawImage(0, 137);
    case 10: drawImage(2, 67);
      break;
    case 9: drawImage(2, 137);
    s = 1;
    case 8: drawImage(0, 0);
    case 7: drawImage(1, 67);
      break;
    case 6: drawImage(0, 67);
    case 5: drawImage(2, 0);
    case 4: drawImage(1, 137);
      break;
    case 3: drawImage(2, 137);
    s = 1;
    case 2: drawImage(1, 67);
    case 1: drawImage(0, 0);
      break;
  }
  p++; if(p==13) p = 0;
  shiftPixelData();
  _delay_ms(400);
  if(s==1) {
    _delay_ms(2000);
  }
}

int main() {
  setup();
  while(1) loop();
}
