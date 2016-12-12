#include "Hardware.h"

Ticker inputTicker;
CRGB strip[NUM_LEDS];
uint8_t brightness = 255;
bool buttonPushed = false;

void initHardware()
{
	pinMode(BUTTON_PIN, INPUT);
	inputTicker.attach_ms(10, handleInput);
	FastLED.addLeds<WS2812B, LIGHTSTRIP_PIN, GRB>(strip, NUM_LEDS);
	FastLED.setDither(0);
}

void handleInput()
{
	// // Apply effect speed if poti value has changed
	// uint8_t newPotiSpeed = analogRead(A0) / 100;
	// if(newPotiSpeed != potiSpeed)
	// {
	// 	setSpeed(newPotiSpeed);
	// 	potiSpeed = newPotiSpeed;
	// }

	// Push button
	if(digitalRead(BUTTON_PIN) && !buttonPushed)
	{
		// button pushed
		cycleEffect();

		buttonPushed = true;
	}
	else if(!digitalRead(BUTTON_PIN) && buttonPushed)
	{
		// button released

		buttonPushed = false;
	}
}