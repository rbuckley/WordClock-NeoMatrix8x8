/*
 * WORD CLOCK - 8x8 NeoPixel Desktop Edition
 * by Andy Doro
 * 
 * A word clock using NeoPixel RGB LEDs for a color shift effect.
 * 
 * Hardware: 
 * - Trinket Pro 5V (should work with other Arduino-compatibles with minor modifications)
 * - DS1307 RTC breakout
 * - NeoPixel NeoMatrix 8x8
 * 
 * 
 * Software:
 * 
 * This code requires the following libraries:
 * 
 * - RTClib https://github.com/adafruit/RTClib
 * - Adafruit_GFX https://github.com/adafruit/Adafruit-GFX-Library
 * - Adafruit_NeoPixel https://github.com/adafruit/Adafruit_NeoPixel
 * - Adafruit_NeoMatrix https://github.com/adafruit/Adafruit_NeoMatrix
 * 
 * 
 * Wiring: 
 * - Solder DS1307 breakout to Trinket Pro, A2 to GND, A3 to PWR, A4 to SDA, A5 to SCL
 * If you leave off / clip the unused SQW pin on the RTC breakout, the breakout can sit right on top of the Trinket Pro 
 * for a compact design! It'll be difficult to reach the Trinket Pro reset button, but you can activate the bootloader by 
 * plugging in the USB.
 * - Solder NeoMatrix 5V to Trinket 5V, GND to GND, DIN to Trinket Pro pin 8.
 * 
 * 
 * grid pattern
 * 
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
 *  
 *  
 *  Acknowledgements:
 *  - Thanks Dano for faceplate / 3D models & project inspiration!
 *  
 */

// include the library code:
#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

// define how to write each of the words

// matrix of 12 - 16 bit ints (shorts) for displaying the leds
uint16_t mask[12];

// using masks for each row 16 bit mask which uses the msb as a row index and the
// next three bytes for the columnn index 
// TODO only using 3 of 4 bytes, maybe put color information in the first byte 
#define IT       mask[0] |= 0xC00     
#define IS       mask[0] |= 0x180
#define HALF     mask[0] |= 0x03C
#define BDAY     mask[1] |= 0xF00
#define QUARTER  mask[1] |= 0x0FE
#define TWENTY   mask[2] |= 0xFC0
#define MFIVE    mask[2] |= 0x01E
#define MTEN     mask[3] |= 0x380
#define PAST     mask[3] |= 0x03C
#define TO       mask[3] |= 0x003
#define TEN      mask[4] |= 0xE00
#define SIX      mask[4] |= 0x1C0
#define ONE      mask[4] |= 0x00E
#define NINE     mask[5] |= 0xF00
#define THREE    mask[5] |= 0x01F
#define EIGHT    mask[6] |= 0xF80
#define FIVE     mask[6] |= 0x078
#define TWO      mask[6] |= 0x007
#define FOUR     mask[7] |= 0x780
#define ELEVEN   mask[7] |= 0x03F
#define SEVEN    mask[8] |= 0xF80
#define TWELVE   mask[8] |= 0x03F
#define OCLOCK   mask[9] |= 0xFC0
#define KATIE    mask[9] |= 0x07C
#define IN       mask[9] |= 0x003
#define THE      mask[10] |= 0xE00
#define MORNING  mask[10] |= 0X0Fe
#define EVENING  mask[11] |= 0x7F0
#define MONE     mask[11] |= 0x001  // one minute past
#define MTWO     mask[11] |= 0x003  // two minutes past
#define MTHREE   mask[11] |= 0x007  // three minutes past
#define MFOUR    mask[11] |= 0x00F  // four minutes past

// define pins
#define NEOPIN 8  // connect to DIN on NeoMatrix 8x8
//#define RTCGND A2 // use this as DS1307 breakout ground TODO not using a RTC yet
//#define RTCPWR A3 // use this as DS1307 breakout power  TODO


// brightness based on time of day- could try warmer colors at night?
#define DAYBRIGHTNESS 40
#define NIGHTBRIGHTNESS 20

// cutoff times for day / night brightness. feel free to modify.
#define MORNINGCUTOFF 7  // when does daybrightness begin?   7am
#define NIGHTCUTOFF   22 // when does nightbrightness begin? 10pm


// define delays
#define FLASHDELAY 250  // delay for startup "flashWords" sequence
#define SHIFTDELAY 100   // controls color shifting speed


//RTC_DS1307 RTC; // Establish clock object
//DateTime theTime; // Holds current clock time

int j;   // an integer for the color shifting effect

// Do you live in a country or territory that observes Daylight Saving Time? 
// https://en.wikipedia.org/wiki/Daylight_saving_time_by_country
// Use 1 if you observe DST, 0 if you don't. This is programmed for DST in the US / Canada. If your territory's DST operates differently, 
// you'll need to modify the code in the calcTheTime() function to make this work properly.
#define OBSERVE_DST 1


// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//Adafruit_NeoPixel matrix = Adafruit_NeoPixel(64, NEOPIN, NEO_GRB + NEO_KHZ800);

// configure for 8x8 neopixel matrix
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(12, 12, NEOPIN,
                            NEO_MATRIX_TOP  + NEO_MATRIX_LEFT +
                            NEO_MATRIX_ROWS + NEO_MATRIX_PROGRESSIVE,
                            NEO_GRB         + NEO_KHZ800);


void setup() {
  // put your setup code here, to run once:

  //Serial for debugging
  //Serial.begin(9600);

  // set pinmodes
  pinMode(NEOPIN, OUTPUT);

  // set analog pins to power DS1307 breakout!
  pinMode(RTCGND, OUTPUT); // analog 2
  pinMode(RTCPWR, OUTPUT); // analog 3

  // set them going!
  digitalWrite(RTCGND, LOW);  // GND for RTC
  digitalWrite(RTCPWR, HIGH); // PWR for RTC

  // start clock
#ifdef USE_RTC
  Wire.begin();  // Begin I2C
  RTC.begin();   // begin clock

  if (! RTC.isrunning()) {
    //Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
    // add 2.5 minutes to get better estimates
    theTime = RTC.now();
    theTime = theTime.unixtime() + 150;
    // DST? If we're in it, let's subtract an hour from the RTC time to keep our DST calculation correct. This gives us 
    // Standard Time which our DST check will add an hour back to if we're in DST. 
    theTime = theTime.unixtime() - 3600; // If we're not in DST right now, just comment this out! If you don't use DST comment this out.
    RTC.adjust(theTime);
  }
#endif

  matrix.begin();
  matrix.setBrightness(DAYBRIGHTNESS);
  matrix.fillScreen(0); // Initialize all pixels to 'off'
  matrix.show();

  // startup sequence... do colorwipe?
  // delay(500);
  // rainbowCycle(20);
  delay(500);
  flashWords(); // briefly flash each word in sequence
  delay(500);
}

void loop() {
  // put your main code here, to run repeatedly:

  adjustBrightness();
  displayTime();

  //mode_moon(); // uncomment to show moon mode instead!


}


