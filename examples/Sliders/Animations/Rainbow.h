#pragma once

#include <FastLEDHub.h>

extern CRGB leds[];

class Rainbow : public Animation
{
public:
  using Animation::Animation;

	void reset()
	{
	}

	void loop()
	{
		// Since we use beatsin88 to generate a sine wave we need to incorporate the speed slider without using delay():
		uint16_t speed = FastLEDHub.getSlider("Speed")->value;
		uint16_t startHue = beatsin16(speed / 16 / 5, 0, 1535);
		uint16_t endHue = beatsin16(speed / 16 / 2, 0, 1535);

		if (startHue < endHue)
		{
			for(int16_t i = 0; i < FastLEDHub[0].size(); i++)
			{
				leds[i] = hsv2rgb_smooth(startHue + (endHue - startHue) * i / FastLEDHub[0].size(), FastLEDHub.getSlider("Saturation")->value, 255);
			}
		}
		else
		{
			for(int16_t i = FastLEDHub[0].size() - 1; i >= 0; i--)
			{
				leds[i] = hsv2rgb_smooth(startHue + (endHue - startHue) * i / FastLEDHub[0].size(), FastLEDHub.getSlider("Saturation")->value, 255);
			}
		}

		  FastLEDHub.show();
	}
};
