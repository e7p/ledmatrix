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

#define LEFT_PIN PA0
#define RIGHT_PIN PA1
#define UP_PIN PA2
#define DOWN_PIN PA3

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
  0x0CC0, 0x0CC0, 0x0CC0, 0x0CC0
};

uint16_t random;
uint8_t gameTick;

uint8_t buttonValue = 0;
uint8_t buttonStatus = 0;

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

void setup() {
  DDRC |= (1<<DATA_LOW)|(1<<DATA_HIGH)|(1<<OE)|(1<<SRCK)|(1<<RCK);
  
  // Initialize Timers
  // 8-bit Timer 0 at 8.16ms
  TCCR0B |= (1<<CS02); // Prescaler 256
  // 16-bit Timer 1 at ?
  TCCR1B |= (1<<CS00); // Prescaler 1

  // Interrupts
  TIMSK0 |= (1<<TOIE0);
  TIMSK1 |= (1<<TOIE1);
  sei();
  
  DDRA &= ~((1<<LEFT_PIN)|(1<<RIGHT_PIN)|(1<<UP_PIN)|(1<<DOWN_PIN));
  PORTA |= (1<<LEFT_PIN)|(1<<RIGHT_PIN)|(1<<UP_PIN)|(1<<DOWN_PIN);
  
  screen_on();
}

uint8_t x = 202;
int8_t y = 6;

void drawTile(uint8_t x, uint8_t y, uint16_t* tile, uint8_t value) {
  if(value == 1) {
    if(x < 203) panelData[x-3] |= (*tile >> 12) << y >> -y;
    if(x < 202) panelData[x-2] |= ((*tile & 0x0f00) >> 8) << y >> -y;
    if(x < 201) panelData[x-1] |= ((*tile & 0x00f0) >> 4) << y >> -y;
    if(x < 200) panelData[x] |= (*tile & 0x000f) << y >> -y;
  } else {
    if(x < 203) panelData[x-3] &= ~((*tile >> 12) << y >> -y);
    if(x < 202) panelData[x-2] &= ~(((*tile & 0x0f00) >> 8) << y >> -y);
    if(x < 201) panelData[x-1] &= ~(((*tile & 0x00f0) >> 4) << y >> -y);
    if(x < 200) panelData[x] &= ~((*tile & 0x000f) << y >> -y);
  }
}

int collideTile(uint8_t x, uint8_t y, uint16_t* tile) {
  uint16_t coll = *tile >> 12;
  uint16_t mask = coll;
  if((coll && x == 3) || (x < 204 && (panelData[x-4] << -y >> y) & 0x0f & coll)) return 1;
  coll = ~mask & ((*tile & 0x0f00) >> 8);
  mask |= coll;
  if((coll && x == 2) || (x < 203 && (panelData[x-3] << -y >> y) & 0x0f & coll)) return 2;
  coll = ~mask & ((*tile & 0x00f0) >> 4);
  mask |= coll;
  if((coll && x == 1) || (x < 202 && (panelData[x-2] << -y >> y) & 0x0f & coll)) return 4;
  coll = ~mask & (*tile & 0x000f);
  mask |= coll;
  if((coll && x == 0) || (x < 201 && (panelData[x-1] << -y >> y) & 0x0f & coll)) return 8;
  return 0;
}

int block = 0;
int speed = 32;
int n = 0, logt=1;

void loop() {
  if(gameTick > 64) {
    //random += buttonValue * 2 + 1;
    
    if(!collideTile(x, y, &blocks[block])) {
      drawTile(x,y,&blocks[block],0);
      x--;
    } else {
      // TODO: erase full lines
      //panelData[(n++)] |= collideTile(x, y, &blocks[block]) | (block << 10) | ((x>180) << 15);
      x = 202;
      y = 6;
      random++;
      block = random % 28;
      if(block > 28) block = 28;
    }
    if(buttonValue & (1<<LEFT_PIN)) {
      // TODO: collideY
      y++;
    }
    if(buttonValue & (1<<RIGHT_PIN)) {
      // TODO: collideY
      y--;
    }
    if(buttonValue & (1<<UP_PIN)) {
      // TODO: collideX & Y
      block = (block / 4) * 4 + (block + 1) % 4;
    }
    if(buttonValue & (1<<DOWN_PIN)) {
      while(!collideTile(x, y, &blocks[block])) {
        x--;
      }
    }
    buttonValue &= ~((1<<LEFT_PIN) | (1<<RIGHT_PIN) | (1<<UP_PIN) | (1<<DOWN_PIN));
    //random /= 3;
    drawTile(x,y,&blocks[block],1);
    shiftPixelData();
    gameTick-=64; // 16=normal
  }
}

ISR(TIMER0_OVF_vect) {
  // Button timer (including simple debounce)
  buttonValue |= PINA & ~buttonStatus | buttonValue;
  buttonStatus = PINA | buttonStatus & buttonValue;
}

ISR(TIMER1_OVF_vect) {
  // Game timer
  gameTick++;
  //random++;
}

int main() {
  setup();
  while(1) loop();
}
