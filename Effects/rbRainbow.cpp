#include "rbRainbow.h"

Effect rbRainbow = {
	"RG Rainbow",
	rbRainbowNamespace::tick,		// tick
	rbRainbowNamespace::reset,		// reset
	2,								// intervalZeroOffset
	2,								// intervalStepSize
};

namespace rbRainbowNamespace
{

	CRGB get_cyclic_color(uint16_t index)
	{
		if(index < MAX_BETTER_HUE * 1/3)
		{
			return betterHue(MAX_BETTER_HUE * 2/3 + index, Config.saturation);
		}
		else
		{
			return betterHue(MAX_BETTER_HUE - (index - MAX_BETTER_HUE * 1/3), Config.saturation);
		}
	}

	// Division will leave a small remainder but won't be noticable
	uint8_t led_div = 2.0/3 * MAX_BETTER_HUE / NUM_LEDS;
	uint16_t step;

	void reset()
	{
		step = 0;
	}

	void tick()
	{
		// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][] // Exemplary hue steps
		// []            []            []            []            []		  // Exemplary led count
		
		for(uint16_t i = 0; i < NUM_LEDS; i++)
		{
			strip[i] = get_cyclic_color((led_div * i + step) % (MAX_BETTER_HUE * 2/3));
		}

		step++;
		if(step == MAX_BETTER_HUE * 2/3)
			step = 0;
	}

}