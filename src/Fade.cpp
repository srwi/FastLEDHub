#include "Fade.h"

#include "FastLEDHub.h"
#include "SerialOut.h"

#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
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

void initialize()
{
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  getSunsetTime();
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
  int8_t hour = (now->tm_hour + Config.timeZone + Config.summerTime) % 24;

  // Check for alarm
  if (Config.alarmEnabled && hour == Config.alarmHour && now->tm_min == Config.alarmMinute)
  {
    Fade::begin(Fade::FadeMode::ALARM);
  }
  // Check for sunset
  else if (Config.sunsetEnabled && hour == Config.sunsetHour && now->tm_min == Config.sunsetMinute)
  {
    // Only start sunset if all leds are off
    if (FastLEDHub.brightness10 == 0)
    {
      Fade::begin(Fade::FadeMode::SUNSET);
    }
    else // brightness10 > 0
    {
      bool ledsIlluminated = false;
      for (uint16_t i = 0; i < FastLEDHub.numLeds; i++)
      {
        if (FastLEDHub.hardwareLeds[i] != CRGB(0, 0, 0))
        {
          ledsIlluminated = true;
          break;
        }
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
  FastLEDHub.show(fadeBrightness);

  currentFade = fadeMode;

  // Prevent starting fade multiple times
  hasBeenStarted = true;
  hasBeenStartedResetTicker.attach(90, [&]() { hasBeenStarted = false; }); // TODO: Should only be called once?

  if (fadeMode == Fade::FadeMode::ALARM)
  {
    FastLEDHub.begin(FastLEDHub.getAnimation(Config.alarmAnimation));
    fadeTicker.attach_ms(Config.alarmDuration * 60 * 1000 / 1024, tick);
    PRINTLN("[FastLEDHub] Start fade 'Alarm'");
  }
  else if (fadeMode == Fade::FadeMode::SUNSET)
  {
    FastLEDHub.begin(FastLEDHub.getAnimation(Config.sunsetAnimation));
    sunsetMaximumBrightness = FastLEDHub.brightness10;
    fadeTicker.attach_ms(Config.sunsetDuration * 60 * 1000 / sunsetMaximumBrightness, tick);
    PRINTLN("[FastLEDHub] Start fade 'Sunset'");
  }
}

void stop()
{
  fadeTicker.detach();
  currentFade = FadeMode::NONE;
}

void tick()
{
  if (FastLEDHub.status == PAUSED)
    return;

  if (currentFade == Fade::FadeMode::ALARM && fadeBrightness == 1023)
  {
    if (Config.postAlarmAnimation != Config.alarmAnimation)
      FastLEDHub.begin(FastLEDHub.getAnimation(Config.postAlarmAnimation));
    fadeTicker.detach();
    PRINTLN("[FastLEDHub] End fade 'Alarm'");
  }
  else if (currentFade == Fade::FadeMode::SUNSET && fadeBrightness == sunsetMaximumBrightness)
  {
    fadeTicker.detach();
    PRINTLN("[FastLEDHub] End fade 'Sunset'");
  }
  else
  {
    if (fadeBrightness < 1023)
      fadeBrightness++;
    else
      fadeTicker.detach();

    PRINTLN("[FastLEDHub] Fade brightness: " + String(fadeBrightness));
  }

  FastLEDHub.brightness10 = fadeBrightness;
}

void getSunsetTime()
{
  PRINT("[FastLEDHub] Getting sunset time...");

  WiFiClient client;
  HTTPClient http;
  String url = "http://api.sunrise-sunset.org/json?lat=" + String(Config.latitude) + "&lng=" + String(Config.longitude) + "&date=today&formatted=0";
  http.begin(client, url);
  String payload = "";
  if (http.GET() > 0)
    payload = http.getString();
  http.end();

  DynamicJsonDocument doc(2048);
  deserializeJson(doc, payload);
  if(doc.containsKey("results") && doc["results"].containsKey("sunset"))
  {
    String sunset = doc["results"]["sunset"].as<String>();
    Config.sunsetHour = (sunset.substring(11, 13).toInt() + Config.timeZone + Config.summerTime + 24) % 24;
    Config.sunsetMinute = sunset.substring(14, 16).toInt();
    Config.save();
    PRINTLN(" " + String(Config.sunsetHour) + ":" + String(Config.sunsetMinute));
  }
  else
  {
    PRINTLN("failed. Using last known time instead.");
  }
}

} // namespace Fade
