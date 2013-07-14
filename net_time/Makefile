FILE=time.hex
CFLAGS=-mmcu=atmega644 -O2 -std=c99

%.o: %.c
	avr-gcc $(CFLAGS) -o $@ $< ../lib/w5100.c ../lib/ethernet.c ../lib/ledmatrix.c ../lib/font.c my_ethernet.c timelib.c

%.hex: %.o
	avr-objcopy -j .text -j .data -O ihex $< $@

%.s: %.c
	avr-gcc $(CFLAGS) -S -o $@ $< ../lib/w5100.c ../lib/ethernet.c ../lib/ledmatrix.c ../lib/font.c my_ethernet.c timelib.c

all: $(FILE)
asm: $(FILE:.hex=.s)

clean:
	rm -f *.o *.s *.hex

flash: $(FILE)
	avrdude -C /etc/avrdude.conf -p m644 -P usb -c usbasp -U flash:w:$<:a

simulate:
	../simulate.sh $(FILE:.hex=.c)
