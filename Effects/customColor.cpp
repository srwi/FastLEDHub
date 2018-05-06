#include "customColor.h"

Effect customColor = {
	"Color",
	customColorNamespace::tick,		// tick
	customColorNamespace::reset,	// reset
	5,								// intervalZeroOffset
	3,								// intervalStepSize
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
	}

	void tick()
	{
		for(int i=0; i<NUM_LEDS; i++)
		{
			strip[i] = currentCustomColor;
		}
	}

}