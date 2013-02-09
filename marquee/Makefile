all:
	avr-gcc -mmcu=atmega644 -O2 marquee.cpp -o marquee.o
	avr-objcopy -j .text -j .data -O ihex marquee.o marquee.hex
asm:
	avr-gcc -mmcu=atmega644 -O2 -S marquee.cpp -o marquee.s
