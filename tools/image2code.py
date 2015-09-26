#!/usr/bin/env python3
from PIL import Image
import os.path
import sys

if len(sys.argv) < 2 or not os.path.isfile(sys.argv[1]):
	print("Error: no image file supplied")
	sys.exit(1)

im = Image.open(sys.argv[1])

(width, height) = im.size

if height != 16:
	print("Error: Height must be 16")
	sys.exit(2)

px = im.load()

for x in range(0, width):
	row = 0
	for y in range(0, 16):
		row = row << 1
		if px[x,y] != (0, 0, 0):
			row = row | 1
	print("0x" + ("0000" + hex(row)[2:])[-4:].upper() + (", " if x < (width - 1) else "") + ("\n" if (x % 16 == 15) else ""), end="")