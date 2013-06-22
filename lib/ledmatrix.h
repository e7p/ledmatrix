#include <avr/io.h>
void clear(void);
void setPixel(uint16_t x, uint8_t y, uint8_t value);
void setDoubleRow(uint8_t x, uint8_t value);
void setRow(uint8_t x, uint16_t value);
uint16_t getPixel(int x, int y);
void screen_off(void);
void screen_on(void);
void shiftPixelData(void);
void ledmatrix_setup(void);
