/* Uptime for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../main/main.h"
#include <stdio.h>
#include <string.h>
extern uint32_t uptime; // TODO: put into main-header

char buffer[20];
uint32_t last_uptime;

uint8_t uptime_loop(uint8_t mode) {
  if(mode > 0 && uptime != last_uptime) {
    last_uptime = uptime;
    clear();
    sprintf(buffer, "Uptime: %d", uptime);
    writeText(buffer, 2);
    setRow(1, uptime & 0xFFFF);
  }
  if(uptime % 60 == 1) {
    return 1;
  } else {
    return 0;
  }
}

/*int main() {
  ledmatrix_setup();
  while(1) {
    uptime_loop(1);
    shiftPixelData();
  }
}*/
