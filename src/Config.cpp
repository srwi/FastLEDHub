#include "Config.h"
#include "SerialOut.h"

#include <FS.h>


bool ConfigClass::initialize()
{
  if (!SPIFFS.begin())
  {
    PRINTLN("[FastLEDManager] Couldn't mount file system.");
    return false;
  }

  File configFile = SPIFFS.open(configFilename, "r");
  if (!configFile)
  {
    PRINTLN("[FastLEDManager] Opening file '" + configFilename + "' failed.");
    return false;
  }

  String content;
  if (configFile.available())
  {
    content = configFile.readString();
  }

  char json[content.length()];
  content.toCharArray(json, content.length());

  return parseJson(json);
}

bool ConfigClass::parseJson(char *input)
{
  DynamicJsonDocument doc(2048);
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
  if (doc.containsKey("color"))
    color = doc["color"].as<String>();
  if (doc.containsKey("speed"))
    speed = doc["speed"];
  if (doc.containsKey("saturation"))
    saturation = doc["saturation"];

  return !error;
}

DynamicJsonDocument ConfigClass::getJson(DynamicJsonDocument doc)
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
  doc["color"] = color;
  doc["speed"] = speed;
  doc["saturation"] = saturation;

  return doc;
}

String ConfigClass::getJsonString()
{
  DynamicJsonDocument doc(2048);
  doc = getJson(doc);
  String buffer = "";
  serializeJson(doc, buffer);
  return buffer;
}

bool ConfigClass::save()
{
  File configFile = SPIFFS.open(configFilename, "w");
  if (!configFile)
  {
    PRINTLN("[FastLEDManager] Opening file " + configFilename + " for saving failed.");
    return false;
  }

  configFile.println(getJsonString());
  configFile.close();

  return true;
}

ConfigClass Config;
