#include "sunrise.h"

Effect sunrise = {
	"Sunrise",
	sunriseNamespace::tick,			// tick
	sunriseNamespace::reset,		// reset
	1,								// intervalZeroOffset
	0,								// intervalStepSize
};

namespace sunriseNamespace
{
	uint32_t step = 0;
	uint32_t duration = 0;
	uint32_t delay_counter = 0;
	CRGB innerColor, outerColor;

	void reset()
	{
		step = 0;
		delay_counter = 0;
		duration = Config.alarm_duration * 60 * 1000;
	}

	void tick()
	{
		if(delay_counter < duration/(5*256))
		{
			delay_counter++;
			return;
		}
		delay_counter = 0;

		if(step < 256)
		{
			innerColor = blend(CRGB::Red, CRGB(255,33,0), step);
			outerColor = blend(CRGB::Black, CRGB::Red, step);
		}
		else if(step < 2*256)
		{
			innerColor = blend(CRGB(255,33,0), CRGB(255,72,7), step);
			outerColor = blend(CRGB::Red, CRGB(255,33,0), step);
		}
		else if(step < 3*256)
		{
			innerColor = blend(CRGB(255,72,7), CRGB(255,102,24), step);
			outerColor = blend(CRGB(255,33,0), CRGB(255,72,7), step);
		}
		else if(step < 4*256)
		{
			innerColor = blend(CRGB(255,102,24), CRGB(255,147,89), step);
			outerColor = blend(CRGB(255,72,7), CRGB(255,101,23), step);
		}
		else if(step < 5*256)
		{
			innerColor = CRGB(255,147,89);
			outerColor = blend(CRGB(255,101,23), CRGB(255,147,89), step);
		}

		fill_gradient_RGB(strip, 0, outerColor, NUM_LEDS/2, innerColor);
		fill_gradient_RGB(strip, NUM_LEDS/2, innerColor, NUM_LEDS-1, outerColor);

		step++;
	}

}