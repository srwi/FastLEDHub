#include "rbRainbow.h"

EffectConfiguration rbRainbow = {
	rbRainbowNamespace::tick,		// tick
	rbRainbowNamespace::reset,		// reset
	5,								// intervalZeroOffset
	5,								// intervalStepSize
};

namespace rbRainbowNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/

	void reset()
	{
		uint8_t fullRedWidth = NUM_LEDS/27;
		fill_gradient(strip, 0, CHSV(160,255,255), NUM_LEDS/2 - fullRedWidth, CHSV(0,255,255));
		for(uint8_t i = NUM_LEDS/2 - fullRedWidth + 1; i < NUM_LEDS/2 + fullRedWidth; i++)
			strip[i] = CRGB::Red;
		fill_gradient(strip, NUM_LEDS/2 + fullRedWidth, CHSV(0,255,255), NUM_LEDS, CHSV(160,255,255));
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