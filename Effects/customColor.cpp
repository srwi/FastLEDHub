#include "customColor.h"

EffectConfiguration customColor = {
	customColorNamespace::tick,		// tick
	customColorNamespace::reset,	// reset
	100,							// intervalZeroOffset
	0,								// intervalStepSize
};

namespace customColorNamespace
{

	CRGB currentCustomColor = CRGB::Black;

	void reset()
	{
		currentCustomColor = strtol((const char *)&Config.custom_color[0], NULL, 16);
		set(currentCustomColor);
	}

	void set(CRGB color)
	{
		currentCustomColor = color;
		fill_solid(strip, NUM_LEDS, currentCustomColor);
	}

	void tick()
	{

	}

}