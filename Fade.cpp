#include "Fade.h"

FadeMode currentFade = NONE;
Ticker fadeTicker;
uint16_t fadeStep = 0;

void handleFade()
{
	// Return if fading is running
	if(currentFade)
		return;

	// Check for alarm
	if(Config.alarm_enabled && hour() == Config.alarm_hour && minute() == Config.alarm_minute)
	{
		begin("Custom Color");
		startFade(ALARM);
	}
	// Check for sunset
	else if(Config.sunset_enabled && hour() == Config.sunset_hour && minute() == Config.sunset_minute)
	{
		begin("Gradient");
		startFade(SUNSET);
	}
}

void startFade(FadeMode fadeMode)
{
	if(currentFade)
		fadeTicker.detach();

	fadeStep = 0;
	FastLED.setBrightness(0);
	FastLED.show();

	if(fadeMode == ALARM)
	{
		begin(Config.alarm_effect);
		fadeTicker.attach_ms(Config.alarm_duration*60*1000/256, fadeTick);
	}
	else if(fadeMode == SUNSET)
	{
		begin(Config.sunset_effect);
		fadeTicker.attach_ms(Config.sunset_duration*60*1000/256, fadeTick);
	}

	currentFade = fadeMode;
}

void stopFade()
{
	if(currentFade)
	{
		fadeTicker.detach();
		FastLED.setBrightness(255);

		currentFade = NONE;
	}
}

void fadeTick()
{
	if(fadeStep > 255)
	{
		if(currentFade == ALARM)
		{
			begin(Config.post_alarm_effect);
		}
		else
		{
			stopFade();
		}

		return;
	}

	fadeStep++;
	FastLED.setBrightness(fadeStep);
}