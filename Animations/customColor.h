#pragma once

class Color : public Animation
{
	public:
		using Animation::Animation;

		CRGB currentCustomColor = CRGB::Black;

		void reset()
		{
			currentCustomColor = strtol((const char *)&Config.custom_color[0], NULL, 16);
			set(currentCustomColor);
		}

		void set(CRGB color)
		{
			currentCustomColor = color;
		}

		void loop()
		{
			for(int i=0; i<NUM_LEDS; i++)
			{
				strip[i] = currentCustomColor;
			}
		}
};
