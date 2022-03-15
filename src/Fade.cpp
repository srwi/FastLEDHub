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

FadeMode mode = FadeMode::NONE;
uint16_t fadeBrightness = 0;
uint16_t targetBrightness = 0;
Ticker fadeTicker;
Ticker debounce;

void initialize()
{
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  getSunsetTime();
}

void handle()
{
  if (mode != FadeMode::NONE || debounce.active())
    return;

  int8_t hour, minute;
  if (!getTime(&hour, &minute))
    return;

  if (Config.alarmEnabled && hour == Config.alarmHour && minute == Config.alarmMinute)
  {
    Fade::begin(Fade::FadeMode::ALARM);
  }
  else if (Config.sunsetEnabled && hour == Config.sunsetHour && minute == Config.sunsetMinute && FastLEDHub.isDim())
  {
    Fade::begin(Fade::FadeMode::SUNSET);
  }
}

void begin(FadeMode fadeMode)
{
  mode = fadeMode;
  targetBrightness = FastLEDHub.brightness10;
  fadeBrightness = 0;
  FastLEDHub.show(fadeBrightness);

  debounce.once(61, [&](){ debounce.detach(); });

  if (fadeMode == Fade::FadeMode::ALARM)
  {
    FastLEDHub.begin(FastLEDHub.getAnimation(Config.alarmAnimation));
    fadeTicker.attach_ms(Config.alarmDuration * 60 * 1000 / 1024, tick);
    PRINTLN("[FastLEDHub] Start fade 'Alarm'");
  }
  else if (fadeMode == Fade::FadeMode::SUNSET)
  {
    FastLEDHub.begin(FastLEDHub.getAnimation(Config.sunsetAnimation));
    fadeTicker.attach_ms(Config.sunsetDuration * 60 * 1000 / targetBrightness, tick);
    PRINTLN("[FastLEDHub] Start fade 'Sunset'");
  }
}

void stop()
{
  fadeTicker.detach();
  mode = FadeMode::NONE;
}

void tick()
{
  if (FastLEDHub.status == PAUSED)
    return;

  if (mode == Fade::FadeMode::ALARM && fadeBrightness == 1023)
  {
    if (Config.postAlarmAnimation != Config.alarmAnimation)
      FastLEDHub.begin(FastLEDHub.getAnimation(Config.postAlarmAnimation));

    stop();
    PRINTLN("[FastLEDHub] End fade 'Alarm'");
  }
  else if (mode == Fade::FadeMode::SUNSET && fadeBrightness == targetBrightness)
  {
    stop();
    PRINTLN("[FastLEDHub] End fade 'Sunset'");
  }
  else
  {
    fadeBrightness++;
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
    int16_t sunsetHour = (sunset.substring(11, 13).toInt() + Config.timeZone + Config.summerTime + 24) % 24;
    int16_t sunsetMinute = sunset.substring(14, 16).toInt();
    int16_t minutesSinceMidnight = sunsetHour * 60 + sunsetMinute;
    minutesSinceMidnight = (minutesSinceMidnight + Config.sunsetOffset + 1440) % 1440;
    Config.sunsetHour = minutesSinceMidnight / 60;
    Config.sunsetMinute = minutesSinceMidnight % 60;
    Config.save();
    PRINTLN(" " + String(Config.sunsetHour) + ":" + String(Config.sunsetMinute));
  }
  else
  {
    PRINTLN("failed. Using last known time instead.");
  }
}

bool getTime(int8_t *hour, int8_t *minute)
{
  time_t n = time(nullptr);

  if (!n)
    return false;

  tm *now = gmtime(&n);
  *hour = (now->tm_hour + Config.timeZone + Config.summerTime) % 24;
  *minute = now->tm_min;

  return true;
}

} // namespace Fade
