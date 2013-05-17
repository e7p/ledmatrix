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

#define PANELS 5
#define PANELDATA_SIZE (80*PANELS)

#define SINGLE_LINE

char msg[] = "Hallo /dev/tal!!!ÄÖÜßäöü€";
#ifdef SINGLE_LINE
  const uint16_t msg_length = 25;
#else
  const uint16_t msg_length = 12;
#endif
uint8_t c = 0;
uint8_t* pointer;

void getNextFontChar() {
 uint8_t chr = msg[c++];
 if(chr & 0x80) { // simple UTF-8
  pointer = &font[0x2f8]; // DEL if not found
  uint8_t uc_chr[] = {0, 0};
  if((chr & 0xf0) == 0xe0) { // or chr & 0x20
   // 3 byte sequence
   uc_chr[0] = chr << 4;
   chr = (msg[c++] & 0x3f);
   uc_chr[0] |= chr >> 2;
   uc_chr[1] = (chr << 6) | (msg[c++] & 0x3f);
  } else if((chr & 0xe0) == 0xc0) { // or else
   // 2 byte sequence
   chr &= 0x1f;
   uc_chr[0] = chr >> 2;
   uc_chr[1] = (chr << 6) | (msg[c++] & 0x3f);
  }
  // find character in map
  for(int i = 0; i < UNICODE_CHARACTERS * 10; i+= 10) {
   if(unicode[i] == uc_chr[0] && unicode[i+1] == uc_chr[1]) {
    pointer = &unicode[i+2];
    break;
   }
  }
 } else { // ASCII
  pointer = &font[(chr-32)*8];
 }
}

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
  uint16_t value_lowhigh = 0;
  for(int i = 0; i < 8; i++) {
    value_lowhigh |= ((value&1)*3)<<i*2;
    value = value >> 1;
  }
  shiftLine(value_lowhigh>>8, value_lowhigh);
}

void setup() {
  DDRC |= (1<<DATA_LOW)|(1<<DATA_HIGH)|(1<<OE)|(1<<SRCK)|(1<<RCK);
  screen_on();
  getNextFontChar();
}

uint8_t i = 0;
uint16_t cnt = 0;
void loop() {
  if(cnt < 8*msg_length) {
    #ifndef SINGLE_LINE
      shiftLine(font[8*(msg[c]-32)+i], font[8*(msg[c+msg_length]-32)+i]);
    #else
      shiftDouble(*pointer++);
    #endif
    i++;
    if(i==8) {
      getNextFontChar();
      i = 0;
    }
  } else if(cnt < 8*msg_length + 10) {
    PORTC ^= (1<<OE);
    if(PORTC & (1<<OE)) _delay_ms(900); else _delay_ms(400);
  } else if (cnt < 16*msg_length + 10) {
    shiftLine(0, 0);
  } else {
    cnt = -1;
    c = 0;
    getNextFontChar();
    i = 0;
  }
  cnt++;
  //_delay_ms(200);
}

int main() {
  setup();
  while(1) loop();
}
