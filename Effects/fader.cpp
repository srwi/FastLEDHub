#include "fader.h"

EffectConfiguration fader = {
	faderNamespace::tick,       // tick
	faderNamespace::reset,      // reset
	4,                          // intervalZeroOffset
	5,                          // intervalStepSize
};

namespace faderNamespace
{

	uint16_t hue1535 = 0;

	void reset()
	{
		hue1535 = 0;
	}

	void tick()
	{
		if(hue1535 > 1535)
			hue1535 = 0;

		fill_solid(strip, NUM_LEDS, betterHue(hue1535, Config.saturation));
		hue1535++;
	}

}