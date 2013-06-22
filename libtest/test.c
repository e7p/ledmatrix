/* Test for LED-Matrix 2
 * Code by Endres */
#define F_CPU 8000000UL
#include "../lib/ledmatrix.h"
#include "../lib/font.h"

void setup() {
  ledmatrix_setup();
}

void loop() {
  writeText("Dies ist ein Testtext", 0);
  shiftPixelData();
}

int main() {
  setup();
  while(1) loop();
}
