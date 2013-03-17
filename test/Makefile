CFLAGS=-mmcu=atmega644 -O2 -std=c99

%.o: %.c
	avr-gcc $(CFLAGS) -o $@ $<

%.hex: %.o
	avr-objcopy -j .text -j .data -O ihex $< $@

%.s: %.c
	avr-gcc $(CFLAGS) -S -o $@ $<

all: test.hex
asm: test.s

flash: test.hex
	avrdude -C /etc/avrdude.conf -p m644 -P usb -c usbasp -U flash:w:$<:a
