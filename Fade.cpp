#include "Fade.h"

FadeMode currentFade = NONE;
uint16_t fadeBrightness = 0;
uint16_t sunsetMaximumBrightness = 0;
Ticker hasBeenStartedResetTicker;
bool hasBeenStarted = false;

Ticker fadeTicker;

void handleFade()
{
	// Return if fading is running
	if(currentFade || hasBeenStarted)
		return;

	// Check for alarm
	if(Config.alarm_enabled && hour() == Config.alarm_hour && minute() == Config.alarm_minute)
	{
		startFade(ALARM);
	}
	// Check for sunset
	else if(Config.sunset_enabled && hour() == Config.sunset_hour && minute() == Config.sunset_minute)
	{
		// Only start sunset if all leds are off
		if(brightness10 == 0)
		{
			startFade(SUNSET);
		}
		else // brightness10 > 0
		{
			bool stripHasColor = false;
			for(uint16_t i = 0; i < NUM_LEDS; i++)
			{
				if(brightness_corrected_strip[i] != CRGB(0,0,0))
				{
					stripHasColor = true;
					break;
				}
			}

			if(!stripHasColor)
				startFade(SUNSET);
		}
	}
}

void startFade(FadeMode fadeMode)
{
	// Set fade starting point
	fadeBrightness = 1;
	betterShow(fadeBrightness);

	currentFade = fadeMode;
	hasBeenStarted = true;
	hasBeenStartedResetTicker.attach(90, [&](){ hasBeenStarted = false; });

	if(fadeMode == ALARM)
	{
		begin(Config.alarm_effect);
		fadeTicker.attach_ms(Config.alarm_duration*60*1000/1024, fadeTick);
		Serial.println("[Fade] Start fade 'Alarm'");
	}
	else if(fadeMode == SUNSET)
	{
		begin(Config.sunset_effect);
		sunsetMaximumBrightness = brightness10;
		fadeTicker.attach_ms(Config.sunset_duration*60*1000/sunsetMaximumBrightness, fadeTick);
		Serial.println("[Fade] Start fade 'Sunset'");
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

	if(currentFade == ALARM && fadeBrightness == 1023)
	{
		if(Config.post_alarm_effect != Config.alarm_effect)
			begin(Config.post_alarm_effect);
		fadeTicker.detach();
		Serial.println("[Fade] End fade 'Alarm'");
	}
	else if(currentFade == SUNSET && fadeBrightness == sunsetMaximumBrightness)
	{
		fadeTicker.detach();
		Serial.println("[Fade] End fade 'Sunset'");
	}
	else
	{
		if(fadeBrightness < 1023)
			fadeBrightness++;
		else
			fadeTicker.detach();

		Serial.println("[Fade] Fade brightness: " + fadeBrightness);
	}

	brightness10 = fadeBrightness;
}