#include <avr/io.h>
uint8_t* getNextFontChar(void);
uint8_t stringLength(char*);
uint8_t scrollText(char*, uint16_t*);
void writeText(char*, uint8_t);
void writeTextY(char*, uint8_t, uint8_t);
