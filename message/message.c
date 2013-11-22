/* Network Time for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include <avr/interrupt.h>
#include "../lib/ledmatrix.h"
#include "../lib/font.h"
#include "../main/main.h"
#include <string.h>

char message[26];

void message_command(char* args) {
  strncpy(message, args, 25);
}

void message_setup(void) {
  addCommand("message", &message_command);
}

void message_unload(void) {
  delCommand("message");
}

uint8_t message_loop(uint8_t mode) {
  if(mode > 0) {
    clear();
    writeText(message, 0);
  }
  return 0;
}

/*int main() {
  ledmatrix_setup();
  message_setup();
  while(1) {
    message_loop(1);
    shiftPixelData();
  }
}*/
