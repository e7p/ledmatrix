#include <avr/io.h>
uint8_t* getNextFontChar(void);
void writeText(char* txt, uint8_t x);
void writeTextY(char* txt, uint8_t x, uint8_t y);
