#include "customColor.h"

Effect customColor2 = {
	"Farbe 2",
	customColor2Namespace::tick,	// tick
	customColor2Namespace::reset,	// reset
	5,								// intervalZeroOffset
	3,								// intervalStepSize
};

namespace customColor2Namespace
{

	CRGB currentCustomColor = CRGB::Black;
	CRGB currentCustomColor2 = CRGB::Black;
	uint8_t offset = 0;

	void reset()
	{
		currentCustomColor = strtol((const char *)&Config.custom_color2[0], NULL, 16);
		set(currentCustomColor);
	}

	void set(CRGB color)
	{
		currentCustomColor = color;
		CHSV second = rgb2hsv_approximate(currentCustomColor);
		second.hue += 45;
		hsv2rgb_rainbow(second, currentCustomColor2);
	}

	void tick()
	{
		offset++;
		for(int i=0; i<NUM_LEDS; i++)
		{
			float color1_weight = sin8(255.0*i/NUM_LEDS*4 + offset*2);
			CRGB color1 = currentCustomColor;
			color1.nscale8(color1_weight);
			float color2_weight = sin8(255.0*(NUM_LEDS-i)/NUM_LEDS*2 + offset);
			CRGB color2 = currentCustomColor2;
			color2.nscale8(color2_weight);

			strip[i] = blend(color1, color2, 128);
		}
	}

}