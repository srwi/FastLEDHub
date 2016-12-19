#include "fader.h"

EffectConfiguration fader = {
	faderNamespace::tick,		// tick
	faderNamespace::reset,		// reset
	10,							// intervalZeroOffset
	7,							// intervalStepSize
};

namespace faderNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/

	uint8_t hue = 0;

	void reset()
	{
		hue = 0;
	}

	void tick()
	{
		fill_solid(strip, NUM_LEDS, CHSV(hue++,255,255));
		FastLED.show();
	}

}