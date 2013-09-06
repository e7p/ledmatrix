#include <avr/io.h>

#define PANELS 5
#define PANELDATA_SIZE (40*PANELS)

extern uint16_t panelData[PANELDATA_SIZE];
void clear(void);
void setPixel(uint16_t x, uint8_t y, uint8_t value);
void setDoubleRow(uint8_t x, uint8_t value);
void addDoubleRowUp(uint8_t x, uint8_t value, uint8_t y);
void addDoubleRowDown(uint8_t x, uint8_t value, uint8_t y);
void setRow(uint8_t x, uint16_t value);
uint16_t getPixel(int x, int y);
void screen_off(void);
void screen_on(void);
void shiftPixelData(void);
void ledmatrix_setup(void);
