#include "ledmatrix.h"

void drawLine(uint16_t from_x, uint8_t from_y, uint16_t to_x, uint8_t to_y) {
	// m * x + n = y
	// m = y2-y1 / x2-x1
	// n = y1 - m * x1
	if(from_x > to_x) {
		uint16_t tmp = to_x;
		to_x = from_x;
		from_x = tmp;
		uint8_t tmp2 = to_y;
		to_y = from_y;
		from_y = tmp2;
	}
	float m = (float)(to_y - from_y) / (float)(to_x - from_x);
	float n = from_y - m * from_x;
	for(;from_x <= to_x;from_x++) {
		setPixel(from_x, (uint8_t)(m * from_x + n), 1);
	}
}