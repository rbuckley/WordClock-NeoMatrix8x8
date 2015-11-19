 WORD CLOCK - NeoMatrix 8x8 Desktop Edition
================
 by Andy Doro & Dano Wall
 
http://andydoro.com/ 

******Updating to use a 12x12 array of NeoPixels******

A word clock which uses NeoPixel RGB LEDs for a color shifting effect.

For more information, follow this guide: https://learn.adafruit.com/neomatrix-8x8-word-clock/

Hardware:
 
 - Arduino
 - DS1307 RTC breakout https://www.adafruit.com/products/264 (maybe)
 - NeoPixel strip of 150 LEDs 1.25" apart
 - 3D printed housing for led strips
 - laser cut faceplate & enclosure
 
Software:
 
This code requires the following libraries:
 
 - RTClib https://github.com/adafruit/RTClib
 - Adafruit_GFX https://github.com/adafruit/Adafruit-GFX-Library
 - Adafruit_NeoPixel https://github.com/adafruit/Adafruit_NeoPixel
 - Adafruit_NeoMatrix https://github.com/adafruit/Adafruit_NeoMatrix


Wiring:

 - TODO
 

grid pattern:
 ```
I	T	A	I	S	B	H	A	L	F	S	T
B	D	A	Y	Q	U	A	R	T	E	R	N
T	W	E	N	T	Y	P	F	I	V	E	R
T	E	T	E	N	F	P	A	S	T	T	O
T	E	N	S	I	X	Y	Y	O	N	E	B
N	I	N	E	A	M	M	T	H	R	E	E
E	I	G	H	T	F	I	V	E	T	W	O
P	F	O	U	R	L	E	L	E	V	E	N
S	E	V	E	N	I	T	W	E	L	V	E
O'	C	L	O	C	K	A	T	I	E	I	N
T	H	E	K	M	O	R	N	I	N	G	J
Q	E	V	E	N	I	N	G	.	.	.	.
 ```
 
Acknowledgements:
  - Thanks Dano for faceplate / 3D models & project inspiration! https://github.com/danowall
 

