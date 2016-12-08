#include "Fade.h"

FadeMode currentFade = NONE;
Ticker fadeTicker;
uint8_t fadeStep = 0;

void handleFade()
{
	// Return if fading is running
	if(currentFade)
		return;

	fadeStep = 0;

	// Check for alarm
	if(hour() == Config.alarm_hour && minute() == Config.alarm_minute)
	{
		currentFade = ALARM;
		fadeTicker.attach_ms(Config.alarm_duration*60*1000/256, fadeTick);
	}
	else if(hour() == Config.sunset_hour && minute() == Config.sunset_minute)
	{
		currentFade = SUNSET;
		fadeTicker.attach_ms(Config.sunset_duration*60*1000/256, fadeTick);
	}
}

void fadeTick()
{
	if(fadeStep > 255)
	{
		fadeTicker.detach();

		if(currentFade == ALARM)
		{
			// TODO: begin post alarm effect
		}

		currentFade = NONE;
		return;
	}

	fadeStep++;
	FastLED.setBrightness(fadeStep);
	// TODO: dithering?
}