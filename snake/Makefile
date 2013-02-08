all:
	avr-gcc -mmcu=atmega644 -O2 snake.cpp -o snake.o
	avr-objcopy -j .text -j .data -O ihex snake.o snake.hex
asm:
	avr-gcc -mmcu=atmega644 -O2 -S snake.cpp -o snake.s
