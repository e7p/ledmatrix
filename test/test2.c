/* Test for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DATA_LOW PC4
#define DATA_HIGH PC2
#define OE PC3
#define SRCK PC6
#define RCK PC7

#define PANELS 2
#define PANELDATA_SIZE (80*PANELS)

uint8_t panelData[PANELDATA_SIZE];

void clear() {
  for(uint8_t i = 0; i < PANELDATA_SIZE; i++) {
    panelData[i] = 0x00;
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

void shiftPixelData(uint8_t data) {
  for(int i = 0; i < PANELDATA_SIZE; i++) {
    uint8_t value_high = data; //panelData[i];
    uint8_t value_low = data; //panelData[++i];
    for(int j = 0; j < 8; j++) {
      PORTC &= ~((1<<DATA_LOW)|(1<<DATA_HIGH));
      PORTC |= ((value_low&1)<<DATA_LOW)|((value_high&1)<<DATA_HIGH);
      value_low = value_low >> 1;
      value_high = value_high >> 1;
      PORTC |= (1<<SRCK);
      PORTC &= ~(1<<SRCK);
    }
  }
  PORTC |= (1<<RCK);
  PORTC &= ~(1<<RCK);
}

void setup() {
  DDRC |= (1<<DATA_LOW)|(1<<DATA_HIGH)|(1<<OE)|(1<<SRCK)|(1<<RCK);
  screen_on();

  // Initialize Timers
  // 8-bit Timer 0 at 8.16ms
  TCCR0B |= (1<<CS02); // Prescaler 256

  // Interrupts
  TIMSK0 |= (1<<TOIE0);
  sei();
}

uint8_t x = 0x0C;
uint8_t y = 1;
uint8_t z = 1;

void loop() {
  x ^= 0x04;
  shiftPixelData(x);
  for(uint8_t i = 0; i < y; i++) {
    _delay_us(40);
  }
}

ISR(TIMER0_OVF_vect) {
  y = (y + 1) % 255;
}

int main() {
  setup();
  while(1) loop();
}