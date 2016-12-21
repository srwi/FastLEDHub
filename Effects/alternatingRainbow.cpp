#include "alternatingRainbow.h"

EffectConfiguration alternatingRainbow = {
	alternatingRainbowNamespace::tick,	// tick
	alternatingRainbowNamespace::reset,	// reset
	5,									// intervalZeroOffset
	0									// intervalStepSize
};

namespace alternatingRainbowNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/

	void reset()
	{
	}

	void tick()
	{
		uint16_t starthue = beatsin16(1, 0, 255);
		uint16_t endhue = beatsin16(3, 0, 255);

		if (starthue < endhue)
		{
			fill_gradient(strip, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), FORWARD_HUES);
		}
		else
		{
			fill_gradient(strip, NUM_LEDS, CHSV(starthue,255,255), CHSV(endhue,255,255), BACKWARD_HUES);
		}

		//FastLED.show();
	}

}