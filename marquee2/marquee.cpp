/* Marquee (scrolling message) for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "font.h"

#define DATA_LOW PC4
#define DATA_HIGH PC2
#define OE PC3
#define SRCK PC6
#define RCK PC7

#define PANELS 1
#define PANELDATA_SIZE (80*PANELS)

#define SINGLE_LINE

// HELLO WORLD
uint8_t msg[] = {40, 37, 44, 44, 47, 55, 47, 50, 44, 36};
#ifdef SINGLE_LINE
  const uint8_t msg_length = 10;
#else
  const uint8_t msg_length = 5;
#endif
uint8_t c = 0;

void screen_off() {
  PORTC &= ~(1<<OE);
}

void screen_on() {
  PORTC |= (1<<OE);
}

void shiftLine(uint8_t value_high, uint8_t value_low) {
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
  PORTC |= (1<<RCK);
  _delay_ms(1);
  PORTC &= ~(1<<RCK);
  _delay_ms(1);
}

void shiftDouble(uint8_t value) {
  uint16_t value_lowhigh;
  for(int i = 0; i < 8; i++) {
    value_lowhigh |= ((value&1)*3)<<i*2;
    value = value >> 1;
  }
  shiftLine(value_lowhigh>>8, value_lowhigh);
}

void setup() {
  DDRC |= (1<<DATA_LOW)|(1<<DATA_HIGH)|(1<<OE)|(1<<SRCK)|(1<<RCK);
  screen_on();
}

uint8_t i = 0;
uint8_t cnt = 0;
void loop() {
  if(cnt < 8*msg_length) {
    #ifndef SINGLE_LINE
      shiftLine(font[8*msg[c]+i], font[8*msg[c+msg_length]+i]);
    #else
      shiftDouble(font[8*msg[c]+i]);
    #endif
    i++;
    if(i==8) {
      i = 0;
      c++;
    }
  } else if(cnt < 8*msg_length + 10) {
    PORTC ^= (1<<OE);
    if(PORTC & (1<<OE)) _delay_ms(900); else _delay_ms(400);
  } else if (cnt < 16*msg_length + 10) {
    shiftLine(0, 0);
  } else {
    cnt = 255;
    c = 0;
    i = 0;
  }
  cnt++;
  //_delay_ms(100);
}

int main() {
  setup();
  while(1) loop();
}
