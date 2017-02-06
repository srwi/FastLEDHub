#include "nox.h"

Effect nox = {
	"Nox",
	noxNamespace::tick,		// tick
	noxNamespace::reset,	// reset
	25,						// intervalZeroOffset
	0						// intervalStepSize
};

namespace noxNamespace
{
	
	void reset()
	{
		clear();
	}

	void tick()
	{
		clear();
	}

}