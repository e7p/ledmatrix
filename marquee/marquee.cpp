/* Marquee (scrolling message) for LED-Matrix "blinkofant"
 * Code by Endres, based on code by wizard23 */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "font.h"

#define CLEAR_PANEL_PIN PB4
// atmega pin #5 <-> panel pin #10

#define DATA_PIN PB5
// atmega pin #6 <-> panel pin #8

#define CLOCK_PIN PB7
// atmega pin #8 <-> panel pin #4

#define PANELS 1
#define PANELDATA_SIZE (10*PANELS)

uint8_t panelData[PANELDATA_SIZE];

// HELLO WORLD!
// 851110211142
//   225 3582 7
uint8_t msg[] = {8, 5, 12, 12, 15, 0, 23, 15, 18, 12, 4, 27};
uint8_t c = 0;

void clear() {
  for(uint8_t i = 0; i < PANELDATA_SIZE; i++) {
    panelData[i] = 0;
  }
}

void setPixel(uint8_t x, uint8_t y, uint8_t value) {
  uint8_t index = (y+1) + x*10; // y+1 because 1st bit controls blinking
  uint8_t byteNum = index >> 3; // division by 8
  uint8_t bitNum = index & 0x7; // remainder at division by 8

  if (value)
    panelData[byteNum] |= 1 << bitNum;
  else
    panelData[byteNum] &= ~(1 << bitNum);
}

uint8_t getPixel(int x, int y) {
  uint8_t index = (y+1) + x*10; // See above
  uint8_t byteNum = index >> 3;
  uint8_t bitNum = index & 0x7;
  return panelData[index >> 3] & (1 << index & 0x7);
}

void screen_off() {
  PORTB &= ~(1<<CLEAR_PANEL_PIN);
}

void screen_on() {
  PORTB |= (1<<CLEAR_PANEL_PIN);
}

void shiftPixelData() {
  screen_off(); // needed?
  
  for(int i = 0; i < PANELDATA_SIZE; i++) {
    uint8_t value = panelData[i];
    
    SPDR = panelData[i];
    while(!(SPSR & (1<<SPIF)));
  }
  
  screen_on(); // needed?
}

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_MODE0 0x00
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_CLOCK_DIV128 0x03
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

void setup() {
  // Initialize serial connection to matrix
  PORTB |= (1<<CLEAR_PANEL_PIN);
  DDRB |= (1<<CLEAR_PANEL_PIN)|(1<<DATA_PIN)|(1<<CLOCK_PIN);
  SPCR |= _BV(MSTR);
  SPCR |= _BV(SPE);
  SPCR |= _BV(DORD);
  SPCR = (SPCR & ~SPI_MODE_MASK) | SPI_MODE0;
  SPCR = (SPCR & ~SPI_CLOCK_MASK) | (SPI_CLOCK_DIV128 & SPI_CLOCK_MASK);
  SPSR = (SPSR & ~SPI_2XCLOCK_MASK) |
    ((SPI_CLOCK_DIV128 >> 2) & SPI_2XCLOCK_MASK);
}

uint8_t s = 0;

void loop() {
  for(uint8_t i = 0; i < 10; i++) {
    //panelData[i] = (font[i+s+8*msg[c+i/(8-s)]-i/5] << (2 * (i % 5)) + 2) | (font[i+s-1+8*msg[c+i/(8-s)]-i/5] >> (10 - 2 * (i % 5)) - 2);
    panelData[i] = (font[i+s+8*msg[c]-i/5] << (2 * (i % 5)) + 2) | (font[i+s-1+8*msg[c]-i/5] >> (10 - 2 * (i % 5)) - 2);
  }
  // first of all inc s, and if s == 8 then inc c
  c++;
  if(c==12) c = 0;
  shiftPixelData();
  _delay_ms(1000);
}

int main() {
  setup();
  while(1) loop();
}
