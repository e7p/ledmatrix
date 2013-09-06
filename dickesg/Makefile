FILE=dickesg.hex
CFLAGS=-mmcu=atmega644 -O2 -std=c99

%.o: %.c
	avr-gcc $(CFLAGS) -o $@ $< ../lib/ledmatrix.c ../lib/font.c

%.hex: %.o
	avr-objcopy -j .text -j .data -O ihex $< $@

%.s: %.c
	avr-gcc $(CFLAGS) -S -o $@ $< ../lib/ledmatrix.c ../lib/font.c

%.bin: %.o
	avr-objcopy -j .text -j .data -O binary $< $@

all: $(FILE)
asm: $(FILE:.hex=.s)
bin: $(FILE:.hex=.bin)

clean:
	rm -f *.o *.s *.hex

flash: $(FILE)
	avrdude -C /etc/avrdude.conf -p m644 -P usb -c usbasp -U flash:w:$<:a

simulate:
	../simulate.sh $(FILE:.hex=.c)