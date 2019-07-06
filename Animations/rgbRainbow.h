#pragma once

class RgbRainbow : public Animation
{
	public:
		using Animation::Animation;

		// Division will leave a small remainder but won't be noticable
		uint8_t led_div = MAX_BETTER_HUE / NUM_LEDS;
		uint16_t step;

		void reset()
		{
			step = 0;
		}

		void loop()
		{
			// [][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][][] // Exemplary hue steps
			// []            []            []            []            []		  // Exemplary led count

			for(uint16_t i = 0; i < NUM_LEDS; i++)
			{
				strip[i] = betterHue((led_div * i + step) % MAX_BETTER_HUE, Config.saturation);
			}

			step++;
			if(step == MAX_BETTER_HUE)
				step = 0;
		}
};
