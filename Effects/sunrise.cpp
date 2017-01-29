#include "sunrise.h"

EffectConfiguration sunrise = {
	sunriseNamespace::tick,			// tick
	sunriseNamespace::reset,		// reset
	10,								// intervalZeroOffset
	0,								// intervalStepSize
};

namespace sunriseNamespace
{
	uint32_t start_time = 0;

	void reset()
	{

	}

	void tick()
	{
		
	}

}