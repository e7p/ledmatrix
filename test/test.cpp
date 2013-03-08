/* Test for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#define DATA_LOW PC4
#define DATA_HIGH PC2
#define OE PC3
#define SRCK PC6
#define RCK PC7

#define PANELS 1
#define PANELDATA_SIZE (80*PANELS)

uint8_t panelData[PANELDATA_SIZE];

void clear() {
  for(uint8_t i = 0; i < PANELDATA_SIZE; i++) {
    panelData[i] = 0;
  }
}

void setPixel(uint8_t x, uint8_t y, uint8_t value) {
  if (value)
    panelData[2*x+y/8] |= 1 << (y % 8);
  else
    panelData[2*x+y/8] &= ~(1 << (y % 8));
}

void setRow(uint8_t x, uint8_t y, uint8_t value) {
  panelData[2*x+y] = value;
}

uint8_t getPixel(int x, int y) {
  return panelData[2*x+y/8] & (1 << (y % 8));
}

void screen_off() {
  PORTC &= ~(1<<OE);
}

void screen_on() {
  PORTC |= (1<<OE);
}

void shiftPixelData() {
  for(int i = 0; i < PANELDATA_SIZE; i++) {
    uint8_t value_high = panelData[i];
    uint8_t value_low = panelData[++i];
    for(int j = 0; j < 8; j++) {
      PORTC &= ~((1<<DATA_LOW)|(1<<DATA_HIGH));
      PORTC |= ((value_low&1)<<DATA_LOW)|((value_high&1)<<DATA_HIGH);
      value_low = value_low >> 1;
      value_high = value_high >> 1;
      PORTC |= (1<<SRCK);
      _delay_ms(1);
      PORTC &= ~(1<<SRCK);
      _delay_ms(1);
    }
  }
  PORTC |= (1<<RCK);
  _delay_ms(1);
  PORTC &= ~(1<<RCK);
  _delay_ms(1);
}

void setup() {
  DDRC |= (1<<DATA_LOW)|(1<<DATA_HIGH)|(1<<OE)|(1<<SRCK)|(1<<RCK);
  for(int x = 0; x < 40; x++) {
    setRow(x, 0, 0x03);
    setRow(x, 1, 0xF0);
  }
  screen_on();
}

void loop() {
  shiftPixelData();
  // nothing to do
}

int main() {
  setup();
  while(1) loop();
}
