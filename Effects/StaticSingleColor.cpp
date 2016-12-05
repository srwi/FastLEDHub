#include "staticSingleColor.h"

Effect staticSingleColor = {
	"Static Single Color",								// name
	staticSingleColorNamespace::tick,					// tick
	staticSingleColorNamespace::reset,					// reset
	1,													// speedZeroOffset
	0,													// speedStepSize
	5													// currentInterval (define default value here; will change at runtime)
};

namespace staticSingleColorNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/

	rgb currentStaticSingleColor = { 0, 0, 0 };

	void reset()
	{
		
	}

	void tick()
	{
		writeColor(currentStaticSingleColor);
	}

}