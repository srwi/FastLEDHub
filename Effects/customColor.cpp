#include "customColor.h"

EffectConfiguration customColor = {
	customColorNamespace::tick,		// tick
	customColorNamespace::reset,	// reset
	5,								// intervalZeroOffset
	0,								// intervalStepSize
};

namespace customColorNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/

	CRGB currentCustomColor;
	CRGB newCustomColor;

	void reset()
	{
		currentCustomColor = CRGB::Black;
		newCustomColor = CHSV(0, 255, 20); /////////////////// TESTING / Should be Black actually
	}

	void set(CRGB newColor)
	{
		newCustomColor = newColor;
	}

	void tick()
	{
		if(newCustomColor != currentCustomColor)
		{
			fill_solid(strip, NUM_LEDS, newCustomColor);
			FastLED.show();

			currentCustomColor = newCustomColor;
		}
	}

}