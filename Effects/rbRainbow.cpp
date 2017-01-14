#include "rbRainbow.h"

EffectConfiguration rbRainbow = {
	rbRainbowNamespace::tick,		// tick
	rbRainbowNamespace::reset,		// reset
	1,								// intervalZeroOffset
	0,								// intervalStepSize
};

namespace rbRainbowNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/

	void reset()
	{
		uint8_t fullRedWidth = 87/27;
		fill_gradient(strip, 0, CHSV(160,255,255), 87/2 - fullRedWidth, CHSV(0,255,255));
		for(uint8_t i = 87/2 - fullRedWidth + 1; i < 87/2 + fullRedWidth; i++)
			strip[i] = CRGB::Red;
		fill_gradient(strip, 87/2 + fullRedWidth, CHSV(0,255,255), 87, CHSV(160,255,255));
	}

	void tick()
	{
		// Shift all leds by one
		strip[87 - 1] = strip[0];
		for(int i = 0; i < 87 - 1; i++)
		{
			strip[i] = strip[i + 1];
		}
	
		//FastLED.show();
	}
}