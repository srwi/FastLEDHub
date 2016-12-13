#include "Fade.h"

FadeMode currentFade = NONE;
uint16_t fadeBrightness = 0;
Ticker fadeTicker;

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
	// Set fade starting point
	fadeBrightness = 1;
	FastLED.setBrightness(fadeBrightness);
	FastLED.show();

	currentFade = fadeMode;

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
}

void stopFade()
{
	fadeTicker.detach();
	currentFade = NONE;
}

void fadeTick()
{
	if(status == PAUSED)
		return;

	if(fadeBrightness == 255)
	{
		if(currentFade == ALARM)
		{
			begin(Config.post_alarm_effect);
		}

		fadeTicker.detach();
	}
	else
	{
		fadeBrightness++;
	}

	FastLED.setBrightness(fadeBrightness);
}