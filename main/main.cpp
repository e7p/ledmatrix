/* Main tool for LED-Matrix 2
 * Contains the window manager that splits the display into multiple windows
 * For example it can split up into one marquee window and a time window
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

// TODO: Following will go into EEPROM when it is finished... or... think about it! ;)
uint8_t storeData[] = {
  0x05, 0x02, 0x20, 0x00, 0x01, 0x00, 0x04, 0x54,
  0x45, 0x53, 0x54, 0xA8, 0x00, 0x01, 0x01, 0x18,
  0x44, 0x69, 0x65, 0x73, 0x20, 0x69, 0x73, 0x74,
  0x20, 0x65, 0x69, 0x6E, 0x20, 0x54, 0x65, 0x73,
  0x74, 0x74, 0x65, 0x78, 0x74, 0x21, 0x21, 0x21,
  0x0E, 0x54, 0x45, 0x53, 0x54, 0x30, 0x31, 0x32,
  0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39
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
  
  // TODO Do basically the same as in loop, but just note down the actual window widths if not specified. or edit them in EEPROM (maybe this is preferred as EEPROM is read continuously in the loop function, but 00 00 widths can be edited easily, so it will save computing power)
}

void loop() {
  // TODO code this
  /*
  - check connected displays? maybe, if it does not take too long and if backlink is active or just do this at setup
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
  uint8_t *data = &storeData[1];
  uint8_t windowCount = *data;
  data++;
  uint16_t xOffset = 0, windowWidth;
  for(uint8_t i = 0; i < windowCount; i++) {
    windowWidth = *(uint16_t*)data;
    data += 2;
    switch(*data++) {
      case 1: // marquee2
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
