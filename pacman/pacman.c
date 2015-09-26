/* Pacman animation for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "../lib/ledmatrix.h"
#include "pacman-animation.h"

uint8_t px = 0;

#define PAC_WIDTH 12

void drawPac(uint8_t x, uint8_t frame, uint8_t mirror) {
  uint16_t *s = &pacman_pac[frame*PAC_WIDTH] + (mirror * (PAC_WIDTH - 1));
  uint16_t *m = &pacman_pac_mask[frame*PAC_WIDTH] + (mirror * (PAC_WIDTH - 1));
  for (uint8_t i = 0; i < PAC_WIDTH; i++) {
    if (x+i > PANELDATA_SIZE + PAC_WIDTH) break;
    if (x+i >= PAC_WIDTH) {
      panelData[x+i-PAC_WIDTH] = (panelData[x+i-PAC_WIDTH] | *s) & (*s | *m);
    }
    if (mirror) {
      s--; m--;
    } else {
      s++; m++;
    }
  }
}

#define GHOST_WIDTH 14

void drawGhost(uint8_t x, uint8_t frame, uint8_t mirror) {
  uint16_t *s = &pacman_ghost[frame*GHOST_WIDTH] + (mirror * (GHOST_WIDTH - 1));
  for (uint8_t i = 0; i < GHOST_WIDTH; i++) {
    if (x+i > PANELDATA_SIZE + GHOST_WIDTH) break;
    if (x+i >= GHOST_WIDTH) {
      panelData[x+i-GHOST_WIDTH] = *s;
    }
    if (mirror) {
      s--;
    } else {
      s++;
    }
  }
}

#define PILL_WIDTH 8

void drawPill(uint8_t x) {
  uint16_t *s = pacman_pill;
  for (uint8_t i = 0; i < PILL_WIDTH; i++) {
    if (x+i >= PANELDATA_SIZE) break;
    panelData[x+i] |= *s++;
  }
}

#define PACDEAD_WIDTH 15

void drawPacDead(uint8_t x, uint8_t frame) {
  uint16_t *s = &pacman_pacdead[frame*PACDEAD_WIDTH];
  for (uint8_t i = 0; i < PACDEAD_WIDTH; i++) {
    if (x+i > PANELDATA_SIZE + PACDEAD_WIDTH) break;
    if (x+i >= PACDEAD_WIDTH) {
      panelData[x+i-PACDEAD_WIDTH] = *s;
    }
    s++;
  }
}

uint8_t pa = 0; /* Animation mode */
uint8_t pf = 0; /* Animation frame */
uint8_t pgx = 0; /* Ghost x */

uint8_t pacman_loop(uint8_t mode) {
  if (mode == 0) return 0;
  switch (pa) {
    case 0:
      for (uint8_t i = PAC_WIDTH; i < px; i++) {
        panelData[i-PAC_WIDTH] = 0;
      }
      //drawPill(PANELDATA_SIZE - 16 + PILL_WIDTH / 2);
      if(px >= PAC_WIDTH) {
        if(px % 2 == 0) {
          pgx++;
        }
        drawGhost(pgx, pf, 0);
      }
      drawPac(px, pf, 0);
      if(px < PANELDATA_SIZE) {
        px++;
        pf ^= 0x01;
      } else {
        pf = 0;
        pa++;
      }
      break;
    case 1:
      clear();
      if (px > pgx + GHOST_WIDTH + 4) {
        drawGhost(pgx, px > pgx + GHOST_WIDTH + 32 ? (pf % 2) + 2 : (pf + 2), 1);
      } else {
        drawGhost(pgx, pf % 2, 0);
      }
      drawPac(px, pf % 2, 1);
      if(px > pgx + GHOST_WIDTH) {
        px--;
        if (px > pgx + GHOST_WIDTH + 4) {
          if (px % 4 == 0) {
            pgx--;
          }
        } else if (px % 2 == 0) {
          pgx++;
        }
        pf++;
        if (pf == 4) pf = 0;
      } else {
        pf = 0;
        pa++;
      }
      break;
    case 2:
      clear();
      drawPacDead(px, pf);
      if(pf < 10) {
        pf++;
      } else {
        pf = 0;
        pa++;
      }
      break;
    case 3:
      clear();
      px = 0;
      pa = 0;
      pf = 0;
      pgx = 0;
      return 1;
  }

  _delay_ms(60);
  return 0;
}

/*int main() {
  ledmatrix_setup();
  while(1) {
    pacman_loop(1);
    shiftPixelData();
  }
}*/
