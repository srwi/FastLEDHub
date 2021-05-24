#include "Config.h"

#include <ArduinoJson.h>
#include <FS.h>


bool ConfigClass::parseJSON(char *input)
{
  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, input);

  // Time
  if (doc.containsKey("timeZone"))
    timeZone = doc["timeZone"];
  if (doc.containsKey("summerTime"))
    summerTime = doc["summerTime"];
  if (doc.containsKey("longitude"))
    longitude = doc["longitude"];
  if (doc.containsKey("latitude"))
    latitude = doc["latitude"];
  // Alarm
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
  // Sunset
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
  // Other properties
  if (doc.containsKey("startupAnimation"))
    startupAnimation = doc["startupAnimation"].as<String>();
  if (doc.containsKey("color"))
    color = doc["color"].as<String>();
  if (doc.containsKey("speed"))
    speed = doc["speed"];
  if (doc.containsKey("saturation"))
    saturation = doc["saturation"];

  return !error;
}

String ConfigClass::getJSON()
{
  DynamicJsonDocument doc(2048);

  // Time
  doc["timeZone"] = timeZone;
  doc["summerTime"] = summerTime;
  doc["longitude"] = longitude;
  doc["latitude"] = latitude;
  // Alarm
  doc["alarmEnabled"] = alarmEnabled;
  doc["alarmHour"] = alarmHour;
  doc["alarmMinute"] = alarmMinute;
  doc["alarmDuration"] = alarmDuration;
  doc["alarmAnimation"] = alarmAnimation;
  doc["postAlarmAnimation"] = postAlarmAnimation;
  // Sunset
  doc["sunsetEnabled"] = sunsetEnabled;
  doc["sunsetHour"] = sunsetHour;
  doc["sunsetMinute"] = sunsetMinute;
  doc["sunsetDuration"] = sunsetDuration;
  doc["sunsetOffset"] = sunsetOffset;
  doc["sunsetAnimation"] = sunsetAnimation;
  // Other properties
  doc["startupAnimation"] = startupAnimation;
  doc["color"] = color;
  doc["speed"] = speed;
  doc["saturation"] = saturation;

  String buffer = "";
  serializeJson(doc, buffer);

  return buffer;
}

bool ConfigClass::init()
{
  if (!SPIFFS.begin())
  {
    Serial.println("[FastLEDManager] Couldn't mount file system.");
    return false;
  }

  File configFile = SPIFFS.open(configFilename, "r");
  if (!configFile)
  {
    Serial.println("[FastLEDManager] Opening file '" + configFilename + "' failed.");
    return false;
  }

  String content;
  if (configFile.available())
  {
    content = configFile.readString();
  }

  char json[content.length()];
  content.toCharArray(json, content.length());

  return parseJSON(json);
}

bool ConfigClass::save()
{
  File configFile = SPIFFS.open(configFilename, "w");
  if (!configFile)
  {
    Serial.println("[FastLEDManager] Opening file " + configFilename + " for saving failed.");
    return false;
  }

  configFile.println(getJSON());
  configFile.close();

  return true;
}

ConfigClass Config;
