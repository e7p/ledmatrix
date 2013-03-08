FILE = marquee
all:
	avr-gcc -mmcu=atmega644 -O2 $(FILE).cpp -o $(FILE).o
	avr-objcopy -j .text -j .data -O ihex $(FILE).o $(FILE).hex
asm:
	avr-gcc -mmcu=atmega644 -O2 -S $(FILE).cpp -o $(FILE).s
flash: all
	avrdude -C /etc/avrdude.conf -p m644 -P usb -c usbasp -U flash:w:$(FILE).hex:a
