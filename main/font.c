#include "font.h"

uint8_t* getNextFontChar(uint8_t *data) {
 uint8_t chr = *(data++);
 if(chr & 0x80) { // simple UTF-8
  uint8_t uc_chr[] = {0, 0};
  if((chr & 0xf0) == 0xe0) { // or chr & 0x20
   // 3 byte sequence
   uc_chr[0] = chr << 4;
   chr = (*(data++) & 0x3f);
   uc_chr[0] |= chr >> 2;
   uc_chr[1] = (chr << 6) | (*(data++) & 0x3f);
  } else if((chr & 0xe0) == 0xc0) { // or else
   // 2 byte sequence
   chr &= 0x1f;
   uc_chr[0] = chr >> 2;
   uc_chr[1] = (chr << 6) | (*(data++) & 0x3f);
  } else {
   return &font[0x2f8]; // DEL if not possible
  }
  // find character in map
  for(int i = 0; i < UNICODE_CHARACTERS * 10; i+= 10) {
   if(unicode[i] == uc_chr[0] && unicode[i+1] == uc_chr[1]) {
    return &unicode[i+2];
   }
  }
  return &font[0x2f8]; // DEL if not found
 } else { // ASCII
  return &font[(chr-32)*8];
 }
}
