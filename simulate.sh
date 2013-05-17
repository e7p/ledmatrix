DIR=$PWD
cd ../../simavr/examples/board_ledmatrix/
cat <<EOF > atmega644_ledmatrix.c
// for linker, emulator, and programmer's sake
#include "avr_mcu_section.h"
AVR_MCU(F_CPU, "atmega644");
EOF
cat $DIR/$1 >> atmega644_ledmatrix.c
make && obj-x86_64-linux-gnu/ledmatrix.elf
