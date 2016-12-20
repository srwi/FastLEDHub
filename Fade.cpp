#include "Fade.h"

FadeMode currentFade = NONE;
uint8_t fadeBrightness = 0;
uint8_t sunsetMaximumBrightness = 0;

Ticker fadeTicker;

void handleFade()
{
	// Return if fading is running
	if(currentFade)
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
		if(gammaCorrectedBrightness == 0)
		{
			startFade(SUNSET);
		}
		else // gammaCorrectedBrightness > 0
		{
			bool stripHasColor = false;
			for(uint16_t i = 0; i < NUM_LEDS; i++)
			{
				if(strip[i] != CRGB(0,0,0))
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
	FastLED.setBrightness(fadeBrightness);
	FastLED.show();

	currentFade = fadeMode;

	if(fadeMode == ALARM)
	{
		begin(Config.alarm_effect);
		fadeTicker.attach_ms(Config.alarm_duration*60*1000/256, fadeTick);
		Serial.println("[Fade] Start fade 'Alarm'");
	}
	else if(fadeMode == SUNSET)
	{
		begin(Config.sunset_effect);
		sunsetMaximumBrightness = brightness;
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

	if(currentFade == ALARM && fadeBrightness == 255)
	{
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
		if(fadeBrightness < 255)
			fadeBrightness++;
		else
			fadeTicker.detach();

		Serial.println("[Fade] Fade brightness: " + fadeBrightness);
	}

	//FastLED.setBrightness(fadeBrightness);
	setGammaCorrectedBrightness(fadeBrightness);
}