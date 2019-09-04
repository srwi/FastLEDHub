#include "Fade.h"

FadeMode currentFade = NONE;
uint16_t fadeBrightness = 0;
uint16_t sunsetMaximumBrightness = 0;
Ticker hasBeenStartedResetTicker;
Ticker fadeTicker;
bool hasBeenStarted = false;

IPAddress timeServer(129, 6, 15, 28); // Alternative: 37.34.57.151
unsigned int localPort = 2390; // Alternative: 8888
WiFiUDP Udp;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];

void handleFade()
{
  // Return if fade is running
  if(currentFade || hasBeenStarted)
    return;

  // Check for alarm
  time_t n = time(nullptr);
  struct tm *now = gmtime(&n);
  if(n && Config.alarmEnabled && now->tm_hour == Config.alarmHour && now->tm_min == Config.alarmMinute)
  {
    startFade(ALARM);
  }
  // Check for sunset
  else if (Config.sunsetEnabled && now->tm_hour == Config.sunsetHour && now->tm_min == Config.sunsetMinute)
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

void initTime()
{
  configTime(Config.timeZone * 3600, Config.summerTime * 3600, "pool.ntp.org", "time.nist.gov");

  getSunsetTime();
  Serial.println("[Sunset] Sunset time will be: " + String(Config.sunsetHour) + ":" + String(Config.sunsetMinute));
}

void getSunsetTime()
{
  Serial.print("[Sunset] Getting sunset time...");

  // Request data from sunrise-sunset.org API
  WiFiClient client;
  const char* host = "api.sunrise-sunset.org";
  String url = "/json?lat=" + String(Config.latitude) + "&lng=" + String(Config.longitude) + "&date=today";
  if (!client.connect(host, 80))
  {
    Serial.println("failed! Using fallback time.");
    return;
  }
  client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0)
  {
    if (millis() - timeout > 1500)
    {
      Serial.println("failed! Using fallback time.");
      client.stop();
      return;
    }
  }

  // Read result of GET request
  String result;
  while(client.available())
    result += client.readStringUntil('\r');

  // Crop actual JSON data
  result = result.substring(result.indexOf('{'), result.lastIndexOf('}') + 1);

  // Get sunset time from JSON data
  DynamicJsonDocument doc(2048);
  deserializeJson(doc, result);
  String sunset = "";
  if(doc.containsKey("results") && doc["results"].containsKey("sunset"))
    sunset = doc["results"]["sunset"].as<String>();

  if(sunset == "")
  {
    Serial.println("failed! Using fallback time.");
    return;
  }

  // Generate sunset time from JSON and saved settings
  int8_t hour = 0;
  int16_t minute = 0;
  hour = sunset.substring(0, sunset.indexOf(':')).toInt();
  minute = sunset.substring(sunset.indexOf(':') + 1, sunset.lastIndexOf(':')).toInt();
  hour += Config.timeZone;
  hour += Config.summerTime;
  minute += Config.sunsetOffset;
  if(sunset.endsWith("PM"))
    hour += 12;
  while(minute >= 60)
  {
    minute -= 60;
    hour++;
  }
  while(minute < 0)
  {
    minute += 60;
    hour--;
  }
  while(hour >= 24)
    hour -= 24;
  while(hour < 0)
    hour += 24;

  // Save time as new fallbackTime in case there is no wifi connection next time
  Config.sunsetHour = hour;
  Config.sunsetMinute = minute;
  Config.save(); // TODO: crashes when saving config
  delay(1000); // TODO: Temporary hotfix for crash

  Serial.println("ok!");
}