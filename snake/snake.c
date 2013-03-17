/* Snake Game for LED-Matrix "blinkofant"
 * Code by Endres, based on code by wizard23 */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define CLEAR_PANEL_PIN PB4
// atmega pin #5 <-> panel pin #10

#define DATA_PIN PB5
// atmega pin #6 <-> panel pin #8

#define CLOCK_PIN PB7
// atmega pin #8 <-> panel pin #4 

#define LEFT_PIN PA0
#define RIGHT_PIN PA1

#define PANELS 1
#define PANELDATA_SIZE (10*PANELS)

#define DIR_RIGHT 0
#define DIR_BOTTOM 1
#define DIR_LEFT 2
#define DIR_TOP 3

uint8_t panelData[PANELDATA_SIZE];

uint8_t snakePos[PANELS*8*9][2];
uint8_t foodPos[2];
uint8_t snakeLength;
uint8_t snakeDir;

uint8_t buttonValue;
uint8_t buttonStatus;

uint8_t gameTick;

uint16_t random;

uint8_t cnt;
uint8_t bl;

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

void togglePixel(int x, int y) {
  uint8_t index = (y+1) + x*10; // See above
  uint8_t byteNum = index >> 3;
  uint8_t bitNum = index & 0x7;
  panelData[byteNum] ^= 1 << bitNum;
}

void screen_off() {
  PORTB &= ~(1<<CLEAR_PANEL_PIN);
}

void screen_on() {
  PORTB |= (1<<CLEAR_PANEL_PIN);
}

bool inSnake(uint8_t x, uint8_t y) {
  for(uint8_t i = 0; i < snakeLength; i++) {
    if(snakePos[i][0] == x && snakePos[i][1] == y)
      return true;
  }
  return false;
}

void newFood() {
  do {
    foodPos[0] = random % 8;
    foodPos[1] = random % 9;
    random++;
  } while(inSnake(foodPos[0], foodPos[1]));
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

  // Initialize Timers
  // 8-bit Timer 0 at 8.16ms
  TCCR0B |= (1<<CS02); // Prescaler 256
  // 16-bit Timer 1 at 524.28ms
  TCCR1B = (1<<CS00) | (1<<CS01); // Prescaler 64

  // Interrupts
  TIMSK0 |= (1<<TOIE0);
  TIMSK1 |= (1<<TOIE1);
  sei();

  // Initialize Snake
  snakeLength = 5;
  snakeDir = DIR_RIGHT;

  DDRA &= ~((1<<LEFT_PIN)|(1<<RIGHT_PIN));
  PORTA |= (1<<LEFT_PIN)|(1<<RIGHT_PIN);

  for(int x = 0; x < 8*PANELS; x++) {
    for(int y = 0; y < 9; y++) {
      if(y < 1 && x < snakeLength) {
        snakePos[x][0] = x;
        setPixel(x, y, 1);
      }
    }
  }
  
  newFood();
}

void shiftSnake() {
  for(int i = 0; i <= snakeLength; i++) {
    snakePos[i][0] = snakePos[i+1][0];
    snakePos[i][1] = snakePos[i+1][1];
  }
}

void snakeLeft() {
  snakeDir = (snakeDir + 3) % 4;
}

void snakeRight() {
  snakeDir = (snakeDir + 1) % 4;
}

void loop() {
  if(gameTick > 0) {
    if(buttonValue & (1<<LEFT_PIN)) snakeLeft();
    if(buttonValue & (1<<RIGHT_PIN)) snakeRight();
    buttonValue &= ~((1<<LEFT_PIN) | (1<<RIGHT_PIN));

    snakePos[snakeLength][0] = snakePos[snakeLength-1][0];
    snakePos[snakeLength][1] = snakePos[snakeLength-1][1];
    if(snakeDir == DIR_LEFT) {
      snakePos[snakeLength][0] = snakePos[snakeLength-1][0] - 1;
      if(snakePos[snakeLength][0] == 255) snakePos[snakeLength][0] = 7;
    } else if(snakeDir == DIR_RIGHT) {
      snakePos[snakeLength][0] = snakePos[snakeLength-1][0] + 1;
      if(snakePos[snakeLength][0] >= 8) snakePos[snakeLength][0] = 0;
    } else if(snakeDir == DIR_TOP) {
      snakePos[snakeLength][1] = snakePos[snakeLength-1][1] - 1;
      if(snakePos[snakeLength][1] == 255) snakePos[snakeLength][1] = 8;
    } else { // BOTTOM
      snakePos[snakeLength][1] = snakePos[snakeLength-1][1] + 1;
      if(snakePos[snakeLength][1] >= 9) snakePos[snakeLength][1] = 0;
    }
    
    if(snakePos[snakeLength][0] == foodPos[0] &&
      snakePos[snakeLength][1] == foodPos[1]) {
      snakeLength++;
      newFood();
    } else if(getPixel(snakePos[snakeLength][0], snakePos[snakeLength][1])) {
      // Game over
      //setPixel(0, 0, 1);
    } else {
      //setPixel(snakePos[0][0], snakePos[0][1], 0);
      //setPixel(snakePos[snakeLength][0], snakePos[snakeLength][1], 1);
      shiftSnake();
    }
    // draw Snake
    clear();
    for(uint8_t i = 0; i < snakeLength; i++) {
      setPixel(snakePos[i][0], snakePos[i][1], 1);
    }
    shiftPixelData();
    
    gameTick--;
  }
  if(cnt == 3) {
    setPixel(foodPos[0], foodPos[1], bl % 2);
    bl++;
    shiftPixelData();
    cnt = 0;
  }
  random++;
}

ISR(TIMER0_OVF_vect) {
  // Button timer (including simple debounce)
  buttonValue |= PINA & ~buttonStatus | buttonValue;
  buttonStatus = PINA | buttonStatus & buttonValue;
  cnt++;
}

ISR(TIMER1_OVF_vect) {
  // Game timer
  gameTick++;
}

int main() {
  setup();
  while(1) loop();
}
