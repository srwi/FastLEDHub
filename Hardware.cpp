#include "Hardware.h"

Ticker inputTicker;
bool buttonPushed = false;
uint16_t brightness = 255;
CRGB strip[NUM_LEDS];

void initHardware()
{
	pinMode(BUTTON_PIN, INPUT);
	inputTicker.attach_ms(10, handleInput);
	FastLED.addLeds<WS2812B, LIGHTSTRIP_PIN, GRB>(strip, NUM_LEDS);
	FastLED.setDither(1);
}

void handleInput()
{
	if(!currentFade)
	{
		// Adjust brightness calculation if needed
		brightness = (((1023 - analogRead(A0)) >> 2) - 7) * 1.06;
		if(brightness > 255)
			brightness = 255;
		FastLED.setBrightness(brightness);
	}

	// Push button
	if(digitalRead(BUTTON_PIN) && !buttonPushed)
	{
		// button pushed
		stopFade();
		cycleEffect();

		buttonPushed = true;
	}
	else if(!digitalRead(BUTTON_PIN) && buttonPushed)
	{
		// button released

		buttonPushed = false;
	}
}