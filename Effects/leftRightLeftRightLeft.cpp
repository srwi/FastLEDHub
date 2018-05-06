#include "leftRightLeftRightLeft.h"

Effect leftRightLeftRightLeft = {
	"Left Right Left Right Left",
	leftRightLeftRightLeftNamespace::tick,			// tick
	leftRightLeftRightLeftNamespace::reset,			// reset
	5,												// intervalZeroOffset
	2,												// intervalStepSize
};

namespace leftRightLeftRightLeftNamespace
{

	uint8_t hue = 0;
	bool left = true;
	int16_t currentLed = 0;

	void reset()
	{
		hue = 0;
		left = true;
		currentLed = 0;
	}

	void tick()
	{
		strip[currentLed] = CHSV(hue++, Config.saturation, 255);
		for(int i = 0; i < NUM_LEDS; i++)
			strip[i].nscale8(250);
		currentLed += left ? 1 : -1;
		if(currentLed == NUM_LEDS || currentLed == -1)
			left = !left;
	}
}