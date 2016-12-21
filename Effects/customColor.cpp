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

	CRGB currentCustomColor = CRGB::Black;
	CRGB newCustomColor = CRGB::Black;

	void reset()
	{
		// Prepare to use newCustomColor (next tick() iteration will then apply the current newCustomColor, which is the previously used color)
		currentCustomColor = CRGB::Black;
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

			currentCustomColor = newCustomColor;
		}
		
		//FastLED.show();
	}

}