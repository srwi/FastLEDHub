#include "popFade.h"

EffectConfiguration popFade = {
	popFadeNamespace::tick,	// tick
	popFadeNamespace::reset,	// reset
	10,							// intervalZeroOffset
	7							// intervalStepSize
};

namespace popFadeNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/
	int ranamount = 87;                                           // The higher the number, lowers the chance for a pixel to light up.
	uint8_t thisdelay = 50;                                       // Standard delay value.
	bool boolcolours = 1;                                         // True = random, False = array of colours (up to 10)
	uint8_t numcolours = 2;                                       // Number of colours in the array to display.
	unsigned long colours[10] = {0xff0000, 0x00ff00, 0x0000ff, 0xffffff};  // Just assign the first 3 of these selectable colours.
	uint8_t maxbar = 1;                                           // Maximum # of pixels to turn on in a row
	uint8_t fadeval = 224;                                        // Fade rate

	void reset()
	{
	}

	void tick()
	{
		uint8_t secondHand = (millis() / 1000) % 30;                // Increase this if you want a longer demo.
		static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
		
		// You can change variables, but remember to set them back in the next demo, or they will stay as is.
		if (lastSecond != secondHand) {
			lastSecond = secondHand;
			switch(secondHand) {
				case  0: thisdelay = 50; colours[0] = 0xffffff; numcolours=1; boolcolours=0; maxbar = 1; break;          // Just white twinkles
				case  5: thisdelay = 20; colours[1] = 0xff0000; numcolours=2; boolcolours=0; maxbar = 4; break;          // Add red and make bars
				case 15: thisdelay = 50; boolcolours=1; maxbar=1; break;                                                 // A pile of colours, 1 pixel in length
				case 20: thisdelay = 50; fadeval = 128; break;                                                           // Slow down the fade
				case 25: thisdelay = 50; colours[2]= 0x0000ff; boolcolours=0; numcolours=3; fadeval = 192; maxbar = 6; break; 
				case 30: break;
			}
		}

		unsigned long thiscolour;
		if (ranamount >NUM_LEDS) ranamount = NUM_LEDS;               // Make sure we're at least utilizing ALL the LED's.
		int idex = random16(0, ranamount);

		if (idex < NUM_LEDS) {                                      // Only the lowest probability twinkles will do.
			boolcolours ? thiscolour = random(0, 0xffffff) : thiscolour =  colours[random16(0, numcolours)];     
			int barlen = random16(1,maxbar);
			for (int i = 0; i <barlen; i++) 
				if (idex+i < NUM_LEDS) strip[idex+i] = thiscolour;       // Make sure we don't overshoot the array.
		}
		for (int i = 0; i <NUM_LEDS; i++) strip[i].nscale8(fadeval); // Go through the array and reduce each RGB value by a percentage.
	}

}