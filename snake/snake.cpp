/* Snake Game for LED-Matrix "blinkofant"
 * Code by Endres, based on code by wizard23 */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>

void screen_off();
void screen_on();

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
uint8_t snakeLength;

uint8_t snakeDir;

void setPixel(int x, int y, int value)
{
  uint8_t index = (y+1) + x*10; // y+1 because 1st bit controls blinking
  uint8_t byteNum = index >> 3; // division by 8
  uint8_t bitNum = index & 0x7; // remainder at division by 8

  if (value)
    panelData[byteNum] |= 1 << bitNum;
  else
    panelData[byteNum] &= ~(1 << bitNum);
}

void shiftPixelData()
{
  screen_off(); // needed?
  
  for (int i = 0; i < PANELDATA_SIZE; i++)
  {
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
  SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((SPI_CLOCK_DIV128 >> 2) & SPI_2XCLOCK_MASK);

  // Initialize Snake
  snakeLength = 10;
  snakeDir = DIR_RIGHT;

  DDRA &= ~((1<<LEFT_PIN)|(1<<RIGHT_PIN));
  PORTA |= (1<<LEFT_PIN)|(1<<RIGHT_PIN);

  snakePos[1][0] = 1;
  snakePos[2][0] = 2;

  for (int x = 0; x < 8*PANELS; x++)
  {
    for (int y = 0; y < 9; y++)
    {
      setPixel(x, y, (y < 1 && x < snakeLength));
    }
  }
}

void screen_off()
{
  PORTB &= ~(1<<CLEAR_PANEL_PIN);
}

void screen_on()
{
  PORTB |= (1<<CLEAR_PANEL_PIN);
}

void shiftSnake() {
  for(int i = 0; i <= snakeLength; i++) {
    snakePos[i][0] = snakePos[i+1][0];
    snakePos[i][1] = snakePos[i+1][1];
  }
}

void snakeLeft() {
  snakeDir = (snakeDir + 1) % 4;
}

void snakeRight() {
  snakeDir = (snakeDir + 3) % 4;
}

void loop ()
{
  if(PINA & (1<<LEFT_PIN)) snakeLeft();
  if(PINA & (1<<RIGHT_PIN)) snakeRight();

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
  setPixel(snakePos[snakeLength][0], snakePos[snakeLength][1], 1);
  setPixel(snakePos[0][0], snakePos[0][1], 0);
  shiftSnake();
  shiftPixelData();
  _delay_ms(500);
  return;
}

int main() {
   setup();
   while(1) loop();
}