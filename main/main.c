/* Main tool for LED-Matrix 2
 * Contains the window manager that splits the display into multiple windows
 * For example it can split up into one marquee window and a time window
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
#define PANELDATA_SIZE (40*PANELS)

// TODO: Following will go into EEPROM when it is finished... or... think about it! ;)
// Maybe save it to eeprom per command and reload it only once into RAM every reboot
// to save EEPROM cycles

uint8_t storeData[] = {
  0x05, 0x02, 0x20, 0x00, 0x01, 0x00, 0x04, 0x54,
  0x45, 0x53, 0x54, 0xA8, 0x00, 0x01, 0x01, 0x18,
  0x44, 0x69, 0x65, 0x73, 0x20, 0x69, 0x73, 0x74,
  0x20, 0x65, 0x69, 0x6E, 0x20, 0x54, 0x65, 0x73,
  0x74, 0x74, 0x65, 0x78, 0x74, 0x21, 0x21, 0x21,
  0x0E, 0x54, 0x45, 0x53, 0x54, 0x30, 0x31, 0x32,
  0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39
};

// Must be allocated in setup!
uint8_t variables[4];

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

void setDouble(uint8_t x, uint8_t value) {
  uint16_t value_lowhigh = 0;
  for(int i = 0; i < 8; i++) {
    value_lowhigh |= ((value&1)*3)<<i*2;
    value = value >> 1;
  }
  setRow(x, 0, value_lowhigh>>8);
  setRow(x, 1, value_lowhigh);
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
  
  // Check connected displays? if backlink is active, don't think so because it wouldn't be necessary if stored in EEPROM as it won't be changed much
  // Load EEPROM Content into RAM
  // Edit 00 00 Widths to matching displays connected - calculate the width once.
  // Allocate variable memory. This will be: 2 for each marquee, W*H/64 for each console
  // - for each marquee: preset xpos for centered strings, so it must only be checked if the width is fitting into the window's width
  // Start Clock timer
}

// Timer Interrupt for exactly 1s counts uptime higher and if overflows adds value to the time (each ~18hrs, so it is not as important by now)

uint16_t xOffset = 0, windowWidth;
uint8_t *data, *variable, *tmp_pointer;
uint8_t tmp[16];
void loop() {
  // TODO code this
  /*
  - Go through storeData (start cnt...)
    - ignore 1st byte (PANELS)
    - read 2nd byte into WindowCount
    - xOffset = 0
    - for i = 0; i < WindowCount; i++
      - read word into WindowWidth
      - switch (read byte WindowType)
        - case 1 (marquee)
          - read byte Options
          - for j = 0; j < Options.LineCount (Bit 0); j++
            - read byte into Width
            - if Width * 8 > WindowWidth <- marquee mode enabled
              - TODO: do marquee specific things? are there any?
            - else
              - calculate centered position for text
            - draw text on position, mind Options.LineCount for double height and do not go further than WindowWidth to not override other windows
          - next j
      - xOffset += WindowWidth
    - next i
  - if PixelDataHasChanged: shiftPixelData
  */
  *data = &storeData[2];
  *variable = &variables[0];
  xOffset = 0;
  for(uint8_t i = 0; i < storeData[1]; i++) {
    windowWidth = *(uint16_t*)data;
    data += 2;
    switch(*data++) {
      case 1: // marquee2
      tmp[0] = *(data++); // Options bit
      tmp[1] = *(data++); // Length of 1st line
      for(uint8_t j = 0; j < tmp[1]; j++) {
        // TODO: getNextCharacter
        for(uint8_t k = 0; k < 8; k++) {
          setDouble(xOffset + *variables + j*8 + k, *(tmp_pointer++)); // TODO only if option is like this
        }
      }
      // TODO
      break;
    }
    xOffset += windowWidth;
  }
}

void call(uint16_t tmp) {
  // ...
  if(tmp) return;
}

int main() {
  setup();
  while(1) loop();
}
