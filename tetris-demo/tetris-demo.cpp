/* Tetris demo for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

#define DATA_LOW PC4
#define DATA_HIGH PC2
#define OE PC3
#define SRCK PC6
#define RCK PC7

#define PANELS 2
#define PANELDATA_SIZE (40*PANELS)

uint8_t blocks[] = {
// ### 
//  #  
0xe4,

// ####
//     
0xf0,

// ##  
//  ## 
0xc6,

// ### 
// #   
0xe8,

// ### 
//   # 
0xe2,

//  ## 
// ##  
0x6c,

// ##  
// ##  
0xcc
};

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
  for(int i = 0; i < PANELDATA_SIZE; i++) {
    uint8_t value_high = panelData[i] >> 8;
    uint8_t value_low = panelData[i];
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
}

uint8_t x = 80;
uint8_t y = 8;

void drawTile(uint8_t x, uint8_t y, uint8_t tile, uint8_t value) {
  if(value == 1) {
    panelData[x+1] |= ((tile >> 4) << y);
    panelData[x] |= ((tile & 0x0f) << y);
  } else {
    panelData[x+1] &= ~((tile >> 4) << y);
    panelData[x] &= ~((tile & 0x0f) << y);
  }
}

void loop() {
  if(x > 0 && getPixel(x-1, y+1) == 0) {
    drawTile(x,y,blocks[0],0);
    x--;
  } else {
    x = 80;
  }
  drawTile(x,y,blocks[0],1);
  shiftPixelData();
  _delay_ms(10);
}

int main() {
  setup();
  while(1) loop();
}
