#include "sunrise.h"

Effect sunrise = {
	"Sonnenaufgang",
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