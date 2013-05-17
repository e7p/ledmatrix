FILE=nyan.hex
CFLAGS=-mmcu=atmega644 -O2 -std=c99

%.o: %.c
	avr-gcc $(CFLAGS) -o $@ $<

%.hex: %.o
	avr-objcopy -j .text -j .data -O ihex $< $@

%.bin: %.o
	avr-objcopy -j .text -j .data -O binary $< $@

%.s: %.c
	avr-gcc $(CFLAGS) -S -o $@ $<

all: $(FILE)
asm: $(FILE:.hex=.s)

clean:
	rm -f *.o *.s *.hex

flash: $(FILE)
	avrdude -C /etc/avrdude.conf -p m644 -P usb -c usbasp -U flash:w:$<:a

simulate:
	../simulate.sh $(FILE:.hex=.c)
