#include "font.h"
#include "font_data.h"
#include "ledmatrix.h"
#include <string.h>

char *msg;

uint8_t* getNextFontChar(void) {
 char chr = *(msg++);
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

uint8_t stringLength(char* msg) {
 uint8_t len = 0;
 char chr = *msg;
 while(chr != 0) {
  if(chr & 0x80) {
   if((chr & 0xf0) == 0xe0) {
    msg += 3;
   } else if((chr & 0xe0) == 0xc0) {
    msg += 2;
   } else {
    msg ++;
   }
  } else {
   msg ++;
  }
  len++;
  chr = *msg;
 }
 return len;
}

uint8_t scrollText(char* txt, uint16_t* j) {
  msg = txt;
  if(*j / 8 >= stringLength(txt) + 25) {
    *j = 0;
    return 0;
  }
  uint8_t *chr;
  uint8_t c;
  uint8_t x = 0, m = 0;
  if(*j<200) x = 200 - *j;
  else {
    for(int i = 0; i < *j / 8 - 25; i++) {
      getNextFontChar();
    }
    m = *j % 8;
  }
  while(*(msg) != 0 && x < 200) { // msg+1?
    chr = getNextFontChar() + m;
    for(int i = m; i < 8; i++) {
      setDoubleRow(x++, *(chr++));
    }
    m = 0;
  }
  return 1;
}

void writeText(char* txt, uint8_t x) {
  writeTextY(txt, x, 16);
}

void writeTextY(char* txt, uint8_t x, uint8_t y) {
  msg = txt;
  uint8_t *chr;
  uint8_t c;
  while(*(msg) != 13 && *(msg) != 0 && x < 200) { // msg+1?
    chr = getNextFontChar();
    for(int i = 0; i < 8; i++) {
      if(y < 16) {
        addDoubleRowUp(x++, *(chr++), y);
      } else {
        addDoubleRowDown(x++, *(chr++), y-16);
      }
    }
  }
}
