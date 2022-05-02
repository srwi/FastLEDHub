#include "Config.h"

#include "ColorUtils.h"
#include "SerialOut.h"
#include "FastLEDHub.h"

#if defined(ESP32)
  #include <SPIFFS.h>
#elif defined(ESP8266)
  #include <FS.h>
#endif

#include <EESuspendTimer.h>

bool ConfigClass::initialize()
{
  SUSPEND_TIMER1;
  if (!SPIFFS.begin())
  {
    RESUME_TIMER1;
    PRINTLN("[FastLEDHub] Couldn't mount file system.");
    return false;
  }

  File configFile = SPIFFS.open(m_configFilename, "r");
  if (!configFile)
  {
    RESUME_TIMER1;
    PRINTLN("[FastLEDHub] Opening file '" + m_configFilename + "' failed.");
    return false;
  }
  RESUME_TIMER1;

  String content;
  if (configFile.available())
  {
    content = configFile.readString();
  }

  char json[content.length()];
  content.toCharArray(json, content.length());

  return parseJson(json);
}

bool ConfigClass::parseJson(const char *input)
{
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, input);

  if (doc.containsKey("timeZone"))
    timeZone = doc["timeZone"];
  if (doc.containsKey("summerTime"))
    summerTime = doc["summerTime"];
  if (doc.containsKey("longitude"))
    longitude = doc["longitude"];
  if (doc.containsKey("latitude"))
    latitude = doc["latitude"];
  if (doc.containsKey("alarmEnabled"))
    alarmEnabled = doc["alarmEnabled"];
  if (doc.containsKey("alarmDuration"))
    alarmDuration = doc["alarmDuration"];
  if (doc.containsKey("alarmHour"))
    alarmHour = doc["alarmHour"];
  if (doc.containsKey("alarmMinute"))
    alarmMinute = doc["alarmMinute"];
  if (doc.containsKey("alarmAnimation"))
    alarmAnimation = doc["alarmAnimation"].as<String>();
  if (doc.containsKey("postAlarmAnimation"))
    postAlarmAnimation = doc["postAlarmAnimation"].as<String>();
  if (doc.containsKey("sunsetEnabled"))
    sunsetEnabled = doc["sunsetEnabled"];
  if (doc.containsKey("sunsetHour"))
    sunsetHour = doc["sunsetHour"];
  if (doc.containsKey("sunsetMinute"))
    sunsetMinute = doc["sunsetMinute"];
  if (doc.containsKey("sunsetDuration"))
    sunsetDuration = doc["sunsetDuration"];
  if (doc.containsKey("sunsetOffset"))
    sunsetOffset = doc["sunsetOffset"];
  if (doc.containsKey("sunsetAnimation"))
    sunsetAnimation = doc["sunsetAnimation"].as<String>();
  if (doc.containsKey("startupAnimation"))
    startupAnimation = doc["startupAnimation"].as<String>();
  if (doc.containsKey("sliderValues"))
  {
    sliderValues.clear();
    JsonArray sliderValueArray = doc["sliderValues"].as<JsonArray>();
    for (uint16_t i = 0; i < sliderValueArray.size(); i++)
    {
      sliderValues.add(sliderValueArray[i]);
    }
  }
  if (doc.containsKey("colorPickerValues"))
  {
    colorPickerValues.clear();
    JsonArray colorPickerValueArray = doc["colorPickerValues"].as<JsonArray>();
    for (uint16_t i = 0; i < colorPickerValueArray.size(); i++)
    {
      colorPickerValues.add(hex2rgb(colorPickerValueArray[i]));
    }
  }

  return !error;
}

void ConfigClass::getUserConfigJson(JsonDocument &doc)
{
  doc["timeZone"] = timeZone;
  doc["summerTime"] = summerTime;
  doc["longitude"] = longitude;
  doc["latitude"] = latitude;
  doc["alarmEnabled"] = alarmEnabled;
  doc["alarmHour"] = alarmHour;
  doc["alarmMinute"] = alarmMinute;
  doc["alarmDuration"] = alarmDuration;
  doc["alarmAnimation"] = alarmAnimation;
  doc["postAlarmAnimation"] = postAlarmAnimation;
  doc["sunsetEnabled"] = sunsetEnabled;
  doc["sunsetHour"] = sunsetHour;
  doc["sunsetMinute"] = sunsetMinute;
  doc["sunsetDuration"] = sunsetDuration;
  doc["sunsetOffset"] = sunsetOffset;
  doc["sunsetAnimation"] = sunsetAnimation;
  doc["startupAnimation"] = startupAnimation;
  JsonArray sliderValueArray = doc.createNestedArray("sliderValues");
  for (uint16_t i = 0; i < sliderValues.size(); i++)
  {
    sliderValueArray.add(sliderValues.get(i));
  }
  JsonArray colorPickerValueArray = doc.createNestedArray("colorPickerValues");
  for (uint16_t i = 0; i < colorPickerValues.size(); i++)
  {
    colorPickerValueArray.add(rgb2hex(colorPickerValues.get(i)));
  }
}

void ConfigClass::getApplicationStateJson(JsonDocument &doc)
{
  doc["status"] = String(FastLEDHub.getStatus());
  doc["currentAnimation"] = FastLEDHub.getCurrentAnimationName();
  JsonArray animations = doc.createNestedArray("animations");
  for (uint8_t i = 0; i < FastLEDHub.animations.size(); i++)
  {
    animations.add(FastLEDHub.animations.get(i)->getName());
  }
  JsonArray sliders = doc.createNestedArray("sliders");
  for (uint8_t i = 0; i < FastLEDHub.sliders.size(); i++)
  {
    JsonObject slider = sliders.createNestedObject();
    slider["name"] = FastLEDHub.sliders.get(i)->name;
    slider["min"] = FastLEDHub.sliders.get(i)->min;
    slider["max"] = FastLEDHub.sliders.get(i)->max;
    slider["step"] = FastLEDHub.sliders.get(i)->step;
    slider["value"] = FastLEDHub.sliders.get(i)->value;
    slider["icon"] = FastLEDHub.sliders.get(i)->icon;
  }
  JsonArray colorPickers = doc.createNestedArray("colorPickers");
  for (uint8_t i = 0; i < FastLEDHub.colorPickers.size(); i++)
  {
    JsonObject colorPicker = colorPickers.createNestedObject();
    colorPicker["name"] = FastLEDHub.colorPickers.get(i)->name;
    colorPicker["value"] = rgb2hex(FastLEDHub.colorPickers.get(i)->value);
    colorPicker["icon"] = FastLEDHub.colorPickers.get(i)->icon;
  }
}

String ConfigClass::asString(bool includeApplicationState)
{
  DynamicJsonDocument doc(3072);

  getUserConfigJson(doc);

  if (includeApplicationState)
  {
    getApplicationStateJson(doc);
  }

  String buffer = "";
  serializeJsonPretty(doc, buffer);
  return buffer;
}

bool ConfigClass::save()
{
  SUSPEND_TIMER1;
  File configFile = SPIFFS.open(m_configFilename, "w");
  if (!configFile)
  {
    RESUME_TIMER1;
    PRINTLN("[FastLEDHub] Opening file " + m_configFilename + " for saving failed.");
    return false;
  }

  configFile.println(asString());
  configFile.close();
  RESUME_TIMER1;

  return true;
}

ConfigClass Config;
