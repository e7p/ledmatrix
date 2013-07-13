/* Nyan cat for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

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
      _delay_ms(10);
  for(int i = 0; i < PANELDATA_SIZE; i++) {
    uint8_t value_high = panelData[i] >> 8;
    uint8_t value_low = panelData[i];
    for(int j = 0; j < 8; j++) {
      uint8_t portc = ((value_high&1)<<DATA_HIGH)|(1<<OE); //((value_low&1)<<DATA_LOW)|
      //PORTC &= ~((1<<DATA_LOW)|(1<<DATA_HIGH));
      //PORTC |= ((value_low&1)<<DATA_LOW)|((value_high&1)<<DATA_HIGH);
      value_low = value_low >> 1;
      value_high = value_high >> 1;
      PORTC = portc;
  //    _delay_us(1);
      _delay_ms(10);
      portc |= (1<<SRCK);
      PORTC = portc;
      _delay_ms(10);
      //_delay_us(5);
    }
  }
  PORTC |= (1<<RCK);
  //_delay_us(2);
  //_delay_us(10);
}

void setup() {
  for(uint8_t i = 0; i < 200; i++) {
    if(i%2 == 1) {
      panelData[i] = 0xaaaa;
    } else {
      panelData[i] = 0x5555;
    }
  }
  DDRC = 0xff;
  //DDRC |= (1<<DATA_LOW)|(1<<DATA_HIGH)|(1<<OE)|(1<<SRCK)|(1<<RCK);
  screen_on();
}

void loop() {
  //shiftPixelData();
  PORTC = 0xff;
  _delay_ms(10);
  PORTC = 0x00;
  _delay_ms(10);
}

int main() {
  setup();
  while(1) loop();
}
