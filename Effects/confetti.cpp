#include "confetti.h"

EffectConfiguration confetti = {
	confettiNamespace::tick,	// tick
	confettiNamespace::reset,	// reset
	5,							// intervalZeroOffset
	1							// intervalStepSize
};

namespace confettiNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/
	
	uint8_t  thisfade = 8;		// How quickly does it fade? Lower = slower fade rate.
	int       thishue = 50;		// Starting hue.
	uint8_t   thisinc = 1;		// Incremental value for rotating hues
	uint8_t   thissat = 100;	// The saturation, where 255 = brilliant colours.
	uint8_t   thisbri = 255;	// Brightness of a sequence. Remember, max_bright is the overall limiter.
	int       huediff = 256;	// Range of random #'s to use for hue

	void reset()
	{
		thisfade = 8;
		thishue = 50;
		thisinc = 1;
		thissat = 100;
		thisbri = 255;
		huediff = 256;
	}

	void tick()
	{
		uint8_t secondHand = (millis() / 1000) % 15;                // IMPORTANT!!! Change '15' to a different value to change duration of the loop.
		static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
		if (lastSecond != secondHand) {                             // Debounce to make sure we're not repeating an assignment.
			lastSecond = secondHand;
			switch(secondHand) {
				case  0: thisinc=1; thishue=192; thissat=255; thisfade=2; huediff=256; break;  // You can change values here, one at a time , or altogether.
				case  5: thisinc=2; thishue=128; thisfade=8; huediff=64; break;
				case 10: thisinc=1; thishue=random16(255); thisfade=1; huediff=16; break;      // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
				case 15: break;                                                                // Here's the matching 15 for the other one.
			}
		}

		fadeToBlackBy(strip, NUM_LEDS, thisfade);                    // Low values = slower fade.
		int pos = random16(NUM_LEDS);                               // Pick an LED at random.
		strip[pos] += CHSV((thishue + random16(huediff))/4 , thissat, thisbri);  // I use 12 bits for hue so that the hue increment isn't too quick.
		thishue = thishue + thisinc;                                // It increments here.
	}

}