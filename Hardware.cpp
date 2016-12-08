#include "Hardware.h"

Ticker inputTicker;
CRGB strip[NUM_LEDS];
uint8_t potiSpeed = 0;
bool buttonPushed = false;

void initHardware()
{
	pinMode(BUTTON_PIN, INPUT);
	inputTicker.attach_ms(10, handleInput);
	FastLED.addLeds<WS2812B, LIGHTSTRIP_PIN, GRB>(strip, NUM_LEDS);
}

void handleInput()
{
	// Apply effect speed if poti value has changed
	uint8_t newPotiSpeed = analogRead(A0) / 100;
	if(newPotiSpeed != potiSpeed)
	{
		setSpeed(newPotiSpeed);
		potiSpeed = newPotiSpeed;
	}


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