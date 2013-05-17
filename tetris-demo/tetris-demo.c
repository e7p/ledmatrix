/* Tetris demo for LED-Matrix 2
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

#define PANELS 5
#define PANELDATA_SIZE (40*PANELS)

uint16_t blocks[] = {
//  ### 
//   #  
  0x0E40, 0x4C40, 0x4E00, 0x4640,
// ####
//     
  0x0F00, 0x2222, 0x00F0, 0x4444,
// ##  
//  ## 
  0x0C60, 0x4C80, 0xC600, 0x2640,
// ### 
// #   
  0x4460, 0x0E80, 0xC440, 0x2E00,
// ### 
//   # 
  0x44C0, 0x8E00, 0x6440, 0x0E20,
//  ## 
// ##  
  0x06C0, 0x8C40, 0x6C00, 0x4620,
// ##  
// ##  
  0xCC00, 0xCC00, 0xCC00, 0xCC00
};

uint16_t random;
uint8_t gameTick;

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
  
  // Initialize Timers
  // 16-bit Timer 1 at 10ms
  TCCR1B = (1<<CS00) | (1<<CS01); // Prescaler 64
  OCR1A = 8192;

  // Interrupts
  TIMSK1 |= (1<<OCIE1A);
  sei();
  
  screen_on();
}

int16_t x = 203;
uint8_t y = 8;

void drawTile(uint8_t x, uint8_t y, uint16_t* tile, uint8_t value) {
  if(value == 1) {
    panelData[x-3] |= (*tile >> 12) << y;
    panelData[x-2] |= ((*tile & 0x0f00) >> 8) << y;
    panelData[x-1] |= ((*tile & 0x00f0) >> 4) << y;
    panelData[x] |= (*tile & 0x000f) << y;
  } else {
    panelData[x-3] &= ~((*tile >> 12) << y);
    panelData[x-2] &= ~(((*tile & 0x0f00) >> 8) << y);
    panelData[x-1] &= ~(((*tile & 0x00f0) >> 4) << y);
    panelData[x] &= ~((*tile & 0x000f) << y);
  }
}

int collideTile(uint8_t x, uint8_t y, uint16_t* tile) {
  uint16_t coll = *tile >> 12;
  if((coll && x == 3) || (panelData[x-4] >> y) & 0x0f & coll) return 1;
  coll = ~coll & ((*tile & 0x0f00) >> 8);
  if((coll && x == 2) || (panelData[x-3] >> y) & 0x0f & coll) return 1;
  coll = ~coll & ((*tile & 0x00f0) >> 4);
  if((coll && x == 1) || (panelData[x-2] >> y) & 0x0f & coll) return 1;
  coll = ~coll & (*tile & 0x000f);
  if((coll && x == 0) || (panelData[x-1] >> y) & 0x0f & coll) return 1;
  return 0;
}

int block = 0;

void loop() {
  if(gameTick > 0) {
    if(!collideTile(x, y, &blocks[block])) {
      drawTile(x,y,&blocks[block],0);
      x--;
    } else {
      x = 203;
      y = (random*17+19) % 12;
      block = (random*11+13) % 28;
    }
    drawTile(x,y,&blocks[block],1);
    shiftPixelData();
  }
  random++;
}

ISR(TIMER1_COMPA_vect) {
  // Game timer
  gameTick++;
}

int main() {
  setup();
  while(1) loop();
}
