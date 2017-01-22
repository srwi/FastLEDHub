#include "customColor.h"

EffectConfiguration customColor = {
	customColorNamespace::tick,		// tick
	customColorNamespace::reset,	// reset
	100,							// intervalZeroOffset
	0,								// intervalStepSize
};

namespace customColorNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/

	CRGB currentCustomColor = CRGB::Black;

	void reset()
	{
		currentCustomColor = strtol((const char *)&Config.custom_color[0], NULL, 16);
		set(currentCustomColor);
	}

	void set(CRGB newColor)
	{
		currentCustomColor = newColor;
		fill_solid(strip, NUM_LEDS, currentCustomColor);
	}

	void tick()
	{

	}

}