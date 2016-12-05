#include "Wakeup.h"

Effect wakeup = {
	"Wakeup",											// name
	wakeupNamespace::tick,								// tick
	wakeupNamespace::reset,								// reset
	50,													// speedZeroOffset
	50,													// speedStepSize
	50													// currentInterval (define default value here; will change at runtime)
};

namespace wakeupNamespace
{
	/**********************************
	 ****** START OF EFFECT CODE ******
	 **********************************/

	rgb wakeupColor;
	uint16_t wakeupStep = 0;

	void reset()
	{
		wakeupColor = { Config.wakeup_red, Config.wakeup_green, Config.wakeup_blue };
		wakeupStep = 0;
	}

	void tick()
	{
		rgb currentColor = {
			wakeupColor.red * wakeupStep / COLOR_RANGE,
			wakeupColor.green * wakeupStep / COLOR_RANGE,
			wakeupColor.blue * wakeupStep / COLOR_RANGE
		};
		writeColor(currentColor);
	
		if(wakeupStep < COLOR_RANGE)
		{
			wakeupStep++;
		}
	}

}