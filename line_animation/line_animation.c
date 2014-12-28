/* Line Animation for LED-Matrix
 * Code by Endres */
#define F_CPU 8000000UL
#include <util/delay.h>
#include "../lib/ledmatrix.h"
#include "../lib/draw.h"
#include "../main/main.h"
#include <string.h>

int16_t lx = PANELDATA_SIZE / 2;
int8_t ldx = 4;

#define STEP_WIDTH 10
#define STEPS 8

uint8_t line_animation_loop(uint8_t mode) {
  clear();
  for(uint8_t i = 0; i < STEPS; i++) {
  	drawLine(PANELDATA_SIZE / 2, 0, lx+(i*STEP_WIDTH),15);
  }
  lx += ldx;
  if(lx+(STEPS-1)*STEP_WIDTH >= PANELDATA_SIZE) {
    ldx = -4;
  } else if(lx <= 0) {
  	ldx = 4;
  }
  return 0;
}

/*int main() {
  ledmatrix_setup();
  message_setup();
  while(1) {
    line_animation_loop(1);
    shiftPixelData();
  }
}*/