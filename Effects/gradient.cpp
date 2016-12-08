#include "gradient.h"

EffectConfiguration gradient = {
	gradientNamespace::tick,		// tick
	gradientNamespace::reset,		// reset
	5,								// intervalZeroOffset
	0,								// intervalStepSize
};

namespace gradientNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/

	void reset()
	{
		fill_gradient(strip, 0, CHSV(0,0,50), 86, CHSV(0,0,0));
		FastLED.show();
	}

	void tick()
	{

	}

}