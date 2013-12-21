/* Hot news for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../main/main.h"
#include <string.h>
extern uint32_t uptime; // TODO: put into main-header

char hotnews[26];

void hotnews_command(char* args) {
  strncpy(hotnews, args, 25);
}

void hotnews_setup(void) {
  //strcpy(hotnews, "Sende eine SMS an 9191"); // TEMP
  addCommand("hotnews", &hotnews_command);
}

void hotnews_unload(void) {
  delCommand("hotnews");
}

uint8_t hotnews_counter = 0;

uint8_t hotnews_loop(uint8_t mode) {
  if(mode > 0) {
    clear();
    if(hotnews_counter % 16 < 9)
      writeText(hotnews, 25 * 4 - strlen(hotnews) * 4); // 4 = half width of font
    hotnews_counter++;
    if(hotnews_counter == 128) {
      hotnews_counter = 0;
      return 255;
    }
  } else {
    if(uptime % 60 == 0) { // 1 per minute
      return 255;
    }
  }
  return 0;
}

/*int main() {
  ledmatrix_setup();
  message_setup();
  while(1) {
    hotnews_loop(1);
    shiftPixelData();
  }
}*/
