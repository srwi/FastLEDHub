#pragma once

#include <FastLEDHub.h>

extern CRGB leds[];

class Gradient : public Animation
{
public:
  using Animation::Animation;

	uint16_t NUM_LEDS;

	void reset()
	{
		NUM_LEDS = FastLEDHub[0].size();
	}

	void loop()
	{
		for(uint16_t i = 0; i < NUM_LEDS; i++)
		{
			leds[i] = blend(FastLEDHub.getColorPicker(0)->value, FastLEDHub.getColorPicker(1)->value, 255 * i / NUM_LEDS);
		}

		FastLEDHub.show();
	}
};
