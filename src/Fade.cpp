#include "Fade.h"
#include "FastLEDHub.h"
#include "SerialOut.h"

#if defined(ESP32)
  #include <HTTPClient.h>
#elif defined(ESP8266)
  #include <ESP8266HTTPClient.h>
#endif
#include <ArduinoJson.h>
#include <ESPEssentials.h>
#include <Ticker.h>
#include <time.h>

namespace Fade
{

  namespace
  {

    FadeMode m_mode = FadeMode::NONE;
    uint16_t m_targetBrightness = 0;
    Ticker m_fadeTicker;
    Ticker m_debounceTicker;
    bool m_debounce = false;

    void tick()
    {
      if (FastLEDHub.getStatus() == PAUSED)
        return;

      if (m_mode == FadeMode::ALARM && FastLEDHub.getBrightness() == m_targetBrightness)
      {
        if (Config.postAlarmAnimation != Config.alarmAnimation)
          FastLEDHub.begin(FastLEDHub.getAnimation(Config.postAlarmAnimation));

        stop();
        PRINTLN("[FastLEDHub] End fade 'Alarm'");
      }
      else if (m_mode == FadeMode::SUNSET && FastLEDHub.getBrightness() == m_targetBrightness)
      {
        stop();
        PRINTLN("[FastLEDHub] End fade 'Sunset'");
      }
      else
      {
        FastLEDHub.setBrightness(FastLEDHub.getBrightness() + 1);
        PRINTLN("[FastLEDHub] Fade brightness: " + String(FastLEDHub.getBrightness()));
      }
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

      StaticJsonDocument<1024> doc;
      deserializeJson(doc, payload);
      if (doc.containsKey("results") && doc["results"].containsKey("sunset"))
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

    bool getCurrentTime(int8_t *hour, int8_t *minute)
    {
      time_t n = time(nullptr);

      if (!n)
        return false;

      tm *now = gmtime(&n);
      *hour = (now->tm_hour + Config.timeZone + Config.summerTime) % 24;
      *minute = now->tm_min;

      return true;
    }

  } // namespace

  void initialize()
  {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    getSunsetTime();
  }

  void handle()
  {
    if (m_mode != FadeMode::NONE || m_debounce)
      return;

    int8_t hour, minute;
    if (!getCurrentTime(&hour, &minute))
      return;

    if (Config.alarmEnabled && hour == Config.alarmHour && minute == Config.alarmMinute)
    {
      Fade::begin(FadeMode::ALARM);
    }
    else if (Config.sunsetEnabled && hour == Config.sunsetHour && minute == Config.sunsetMinute && FastLEDHub.isDim())
    {
      Fade::begin(FadeMode::SUNSET);
    }
  }

  void begin(FadeMode fadeMode)
  {
    m_mode = fadeMode;
    uint8_t currentBrightness = FastLEDHub.getBrightness();
    FastLEDHub.setBrightness(0);
    FastLEDHub.show();

    m_debounceTicker.once(61, [](){ m_debounce = false; });
    m_debounce = true;

    if (fadeMode == FadeMode::ALARM)
    {
      m_targetBrightness = 255;
      FastLEDHub.begin(FastLEDHub.getAnimation(Config.alarmAnimation));
      m_fadeTicker.attach_ms(Config.alarmDuration * 60 * 1000 / m_targetBrightness, tick);
      PRINTLN("[FastLEDHub] Start fade 'Alarm'");
    }
    else if (fadeMode == FadeMode::SUNSET)
    {
      m_targetBrightness = currentBrightness;
      FastLEDHub.begin(FastLEDHub.getAnimation(Config.sunsetAnimation));
      m_fadeTicker.attach_ms(Config.sunsetDuration * 60 * 1000 / m_targetBrightness, tick);
      PRINTLN("[FastLEDHub] Start fade 'Sunset'");
    }
  }

  void stop()
  {
    m_fadeTicker.detach();
    m_mode = FadeMode::NONE;
  }

  FadeMode getMode()
  {
    return m_mode;
  }

} // namespace Fade
