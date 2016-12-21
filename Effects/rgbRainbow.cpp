#include "rgbRainbow.h"

EffectConfiguration rgbRainbow = {
	rgbRainbowNamespace::tick,		// tick
	rgbRainbowNamespace::reset,		// reset
	3,								// intervalZeroOffset
	5,								// intervalStepSize
};

namespace rgbRainbowNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/

	void reset()
	{
  		// Make one full rotation on the hue scale
		fill_gradient(strip, 0, CHSV(0,255,255), NUM_LEDS, CHSV(255,255,255), LONGEST_HUES);
	}

	void tick()
	{
		// Shift all leds by one position
		strip[NUM_LEDS - 1] = strip[0];
		for(int i = 0; i < NUM_LEDS - 1; i++)
		{
			strip[i] = strip[i + 1];
		}
	
		//FastLED.show();
	}

}