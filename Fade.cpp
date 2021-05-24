#include "Fade.h"

#include "FastLEDManager.h"

#include <ArduinoJson.h>
#include <ESPEssentials.h>
#include <Ticker.h>
#include <time.h>


namespace Fade
{

FadeMode currentFade = FadeMode::NONE;
uint16_t fadeBrightness = 0;
uint16_t sunsetMaximumBrightness = 0;
Ticker fadeTicker;
Ticker hasBeenStartedResetTicker;
bool hasBeenStarted = false;
bool hasSunsetTime = false;
ConfigClass *config;

void initialize()
{
  configTime(Config.timeZone * 3600, Config.summerTime * 3600, "pool.ntp.org", "time.nist.gov");
}

void handle()
{
  // Return if fade is running
  if (currentFade != FadeMode::NONE || hasBeenStarted)
    return;

  // Get current time
  time_t n = time(nullptr);
  if (!n)
    return;
  struct tm *now = gmtime(&n);

  // Get sunset time if not yet done
  if (!hasSunsetTime && now->tm_year != 70)
  {
    getSunset(now->tm_yday, Config.latitude, Config.longitude);
    hasSunsetTime = true;
  }

  // Check for alarm
  if (Config.alarmEnabled && now->tm_hour == Config.alarmHour && now->tm_min == Config.alarmMinute)
  {
    Fade::begin(Fade::FadeMode::ALARM);
  }
  // Check for sunset
  else if (Config.sunsetEnabled && now->tm_hour == Config.sunsetHour && now->tm_min == Config.sunsetMinute)
  {
    // Only start sunset if all leds are off
    if (FastLEDManager.brightness10 == 0)
    {
      Fade::begin(Fade::FadeMode::SUNSET);
    }
    else // brightness10 > 0
    {
      bool ledsIlluminated = false;
      for (uint16_t i = 0; i < NUM_LEDS; i++)
      {
        // if (brightnessCorrectedLeds[i] != CRGB(0, 0, 0))
        // {
        //   ledsIlluminated = true;
        //   break;
        // }
      }

      if (!ledsIlluminated)
        Fade::begin(Fade::FadeMode::SUNSET);
    }
  }
}

void begin(FadeMode fadeMode)
{
  // Set fade starting point
  fadeBrightness = 1;
  FastLEDManager.show(fadeBrightness);

  currentFade = fadeMode;

  // Prevent starting fade multiple times
  hasBeenStarted = true;
  hasBeenStartedResetTicker.attach(90, [&]() { hasBeenStarted = false; }); // TODO: Should only be called once?

  if (fadeMode == Fade::FadeMode::ALARM)
  {
    FastLEDManager.begin(FastLEDManager.getAnimation(Config.alarmAnimation));
    fadeTicker.attach_ms(Config.alarmDuration * 60 * 1000 / 1024, tick);
    Serial.println("[Fade] Start fade 'Alarm'");
  }
  else if (fadeMode == Fade::FadeMode::SUNSET)
  {
    FastLEDManager.begin(FastLEDManager.getAnimation(Config.sunsetAnimation));
    sunsetMaximumBrightness = FastLEDManager.brightness10;
    fadeTicker.attach_ms(Config.sunsetDuration * 60 * 1000 / sunsetMaximumBrightness, tick);
    Serial.println("[Fade] Start fade 'Sunset'");
  }
}

void stop()
{
  fadeTicker.detach();
  currentFade = FadeMode::NONE;
}

void tick()
{
  if (FastLEDManager.status == PAUSED)
    return;

  if (currentFade == Fade::FadeMode::ALARM && fadeBrightness == 1023)
  {
    if (Config.postAlarmAnimation != Config.alarmAnimation)
      FastLEDManager.begin(FastLEDManager.getAnimation(Config.postAlarmAnimation));
    fadeTicker.detach();
    Serial.println("[Fade] End fade 'Alarm'");
  }
  else if (currentFade == Fade::FadeMode::SUNSET && fadeBrightness == sunsetMaximumBrightness)
  {
    fadeTicker.detach();
    Serial.println("[Fade] End fade 'Sunset'");
  }
  else
  {
    if (fadeBrightness < 1023)
      fadeBrightness++;
    else
      fadeTicker.detach();

    Serial.println("[Fade] Fade brightness: " + String(fadeBrightness));
  }

  FastLEDManager.brightness10 = fadeBrightness;
}

float rad(float deg)
{
  return PI * deg / 180;
}

// https://quantitative-ecology.blogspot.com/2007/10/approximate-sunrise-and-sunset-times.html
void getSunset(uint16_t d, float Lat, float Long)
{
  // This function is copied from:
  // Teets, D.A. 2003. Predicting sunrise and sunset times.
  // The College Mathematics Journal 34(4):317-321.

  // At the default location the estimates of sunrise and sunset are within
  // seven minutes of the correct times (http://aa.usno.navy.mil/data/docs/RS_OneYear.php)
  // with a mean of 2.4 minutes error.

  // Radius of the earth (km)
  float R = 6378;

  // Radians between the xy-plane and the ecliptic plane
  float epsilon = rad(23.45);

  // Convert observer's latitude to radians
  float L = rad(Lat);

  // Calculate offset of sunrise based on longitude (min)
  // If Long is negative, then the mod represents degrees West of
  // a standard time meridian, so timing of sunrise and sunset should
  // be made later.
  float timezone = -4 * (abs(Long) % 15) * (Long >= 0 ? 1 : -1);

  // The earth's mean distance from the sun (km)
  float r = 149598000;

  float theta = 2 * PI / 365.25 * (d - 80);

  float zs = r * sin(theta) * sin(epsilon);
  float rp = sqrt(r * r - zs * zs);

  float t0 = 1440 / (2 * PI) * acos((R - zs * sin(L)) / (rp * cos(L)));

  // A kludge adjustment for the radius of the sun
  float that = t0 + 5;

  // Adjust "noon" for the fact that the earth's orbit is not circular:
  float n = 720 - 10 * sin(4 * PI * (d - 80) / 365.25) + 8 * sin(2 * PI * d / 365.25);

  // Now sunrise and sunset are:
  // float sunrise = (n - that + timezone) / 60;
  float sunset = (n + that + timezone) / 60;

  Config.sunsetHour = (int)floor(sunset) % 24;
  Config.sunsetMinute = (sunset - floor(sunset)) * 60;
  Config.save();

  Serial.println("[Sunset] Got sunset time: " + String(Config.sunsetHour) + ":" + String(Config.sunsetMinute));
}

} // namespace Fade
