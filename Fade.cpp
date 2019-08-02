#include "Fade.h"

FadeMode currentFade = NONE;
uint16_t fadeBrightness = 0;
uint16_t sunsetMaximumBrightness = 0;
Ticker hasBeenStartedResetTicker;
Ticker fadeTicker;
bool hasBeenStarted = false;

void handleFade()
{
  // Return if fade is running
  if(currentFade || hasBeenStarted)
    return;

  // Check for alarm
  if(Config.alarmEnabled && hour() == Config.alarmHour && minute() == Config.alarmMinute)
  {
    startFade(ALARM);
  }
  // Check for sunset
  else if(Config.sunsetEnabled && hour() == Config.sunsetHour && minute() == Config.sunsetMinute)
  {
    // Only start sunset if all leds are off
    if(brightness10 == 0)
    {
      startFade(SUNSET);
    }
    else // brightness10 > 0
    {
      bool stripIsIlluminated = false;
      for(uint16_t i = 0; i < NUM_LEDS; i++)
      {
        if(brightness_corrected_strip[i] != CRGB(0,0,0))
        {
          stripIsIlluminated = true;
          break;
        }
      }

      if(!stripIsIlluminated)
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

  // Prevent starting fade multiple times
  hasBeenStarted = true;
  hasBeenStartedResetTicker.attach(90, [&](){ hasBeenStarted = false; }); // TODO: Should only be called once?

  if(fadeMode == ALARM)
  {
    getAnimation(Config.alarmAnimation)->begin();
    fadeTicker.attach_ms(Config.alarmDuration*60*1000/1024, fadeTick);
    Serial.println("[Fade] Start fade 'Alarm'");
  }
  else if(fadeMode == SUNSET)
  {
    getAnimation(Config.sunsetAnimation)->begin();
    sunsetMaximumBrightness = brightness10;
    fadeTicker.attach_ms(Config.sunsetDuration*60*1000/sunsetMaximumBrightness, fadeTick);
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
    if(Config.postAlarmAnimation != Config.alarmAnimation)
      getAnimation(Config.postAlarmAnimation)->begin();
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

    Serial.println("[Fade] Fade brightness: " + String(fadeBrightness));
  }

  brightness10 = fadeBrightness;
}