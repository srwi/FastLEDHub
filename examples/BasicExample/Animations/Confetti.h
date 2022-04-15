/// This animation has been taken from https://github.com/atuline/FastLED-Demos and
/// modified to be used with FastLEDHub.
/// It is licensed under the GNU General Public License v3.0.

#pragma once

#include <FastLEDHub.h>

extern CRGB leds[];

class Confetti : public Animation
{
public:
	using Animation::Animation;

	uint8_t fade = 8;
	uint8_t inc = 1;
	uint8_t saturation = 100;
	uint8_t brightness = 255;
	int hue = 50;
	int hueDiff = 256;

	void reset()
	{
	}

	void loop()
	{
		uint8_t secondHand = (millis() / 1000) % 15;
		static uint8_t lastSecond = 99;
		if (lastSecond != secondHand)
		{
			lastSecond = secondHand;
			switch(secondHand)
			{
				case 0: inc = 1; hue = 192; saturation = 255; fade = 2; hueDiff = 256; break;
				case 5: inc = 2; hue = 128; fade = 8; hueDiff = 64; break;
				case 10: inc = 1; hue = random16(255); fade = 1; hueDiff = 16; break;
				case 15: break;
			}
		}

		fadeToBlackBy(leds, FastLEDHub[0].size(), fade);
		int pos = random16(FastLEDHub[0].size());
		leds[pos] += CHSV((hue + random16(hueDiff))/4 , saturation, brightness);
		hue = hue + inc;

		FastLEDHub.delay(5);
		FastLEDHub.show();
	}

};
