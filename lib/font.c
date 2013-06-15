#include "font.h"
#include "font_data.h"
#include "ledmatrix.h"

uint8_t* getNextFontChar(uint8_t* msg) {
 uint8_t chr = *(msg++);
 uint8_t* pointer;
 if(chr & 0x80) { // simple UTF-8
  pointer = &font[0x2f8]; // DEL if not found
  uint8_t uc_chr[] = {0, 0};
  if((chr & 0xf0) == 0xe0) { // or chr & 0x20
   // 3 byte sequence
   uc_chr[0] = chr << 4;
   chr = (*(msg++) & 0x3f);
   uc_chr[0] |= chr >> 2;
   uc_chr[1] = (chr << 6) | (*(msg++) & 0x3f);
  } else if((chr & 0xe0) == 0xc0) { // or else
   // 2 byte sequence
   chr &= 0x1f;
   uc_chr[0] = chr >> 2;
   uc_chr[1] = (chr << 6) | (*(msg++) & 0x3f);
  }
  // find character in map
  for(int i = 0; i < UNICODE_CHARACTERS * 10; i+= 10) {
   if(unicode[i] == uc_chr[0] && unicode[i+1] == uc_chr[1]) {
    pointer = &unicode[i+2];
    break;
   }
  }
 } else { // ASCII
  pointer = &font[(chr-32)*8];
 }
 return pointer;
}

void writeText(uint8_t* msg, uint8_t x) {
  uint8_t *chr;
  while(*msg != 0) {
    chr = getNextFontChar(msg);
    for(int i = 0; i < 8; i++) {
      setDoubleRow(x++, *(chr++));
    }
  }
}
