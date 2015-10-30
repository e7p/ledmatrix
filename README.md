ledmatrix
=========

We got a couple of LED matrices in our Hackerspace /dev/tal in Wuppertal. Here there is held code and other useful things for them.

Important info: The Fusebit for "JTAG" MUST be deactivated for usage of PORT C, which is used by default for serial data!

How to build
------------

To build the Firmware, just enter the directory "main" and type `make`. For flashing the firmware `make flash` can be used, or the main.hex file can be flashed using an arbitrary AVR flashing tool.
