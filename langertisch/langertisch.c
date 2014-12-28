/* Langer Tisch 2014 for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../main/main.h"
#include <string.h>

uint8_t langertisch_loop(uint8_t mode) {
  //clear();
  writeText("Test", 0);
  return 0;
}

/*int main() {
  ledmatrix_setup();
  message_setup();
  while(1) {
    langertisch_loop(1);
    shiftPixelData();
  }
}*/
