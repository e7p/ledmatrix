FILE=main.hex
CFLAGS=-mmcu=atmega644 -O2 -std=c99

MODULES=../nyan/nyan.c ../net_time/time.c ../net_time/timelib.c ../net_time/my_ethernet.c ../augenkrebs/augenkrebs.c ../message/message.c ../hotnews/hotnews.c ../langertisch/langertisch.c ../line_animation/line_animation.c ../uptime/uptime.c

%.o: %.c
	avr-gcc $(CFLAGS) -o $@ $< ../lib/w5100.c ../lib/ethernet.c ../lib/ledmatrix.c ../lib/font.c ../lib/draw.c $(MODULES)

%.hex: %.o
	avr-objcopy -j .text -j .data -O ihex $< $@

%.s: %.c
	avr-gcc $(CFLAGS) -S -o $@ $< ../lib/w5100.c ../lib/ethernet.c ../lib/ledmatrix.c ../lib/font.c ../lib/draw.c $(MODULES)

all: $(FILE)
asm: $(FILE:.hex=.s)

clean:
	rm -f *.o *.s *.hex

flash: all
	avrdude -C /etc/avrdude.conf -p m644 -P usb -c usbasp -U flash:w:$(FILE):a

simulate:
	../simulate.sh $(FILE:.hex=.c)
