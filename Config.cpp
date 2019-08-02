#include "Config.h"

bool ConfigClass::parseJSON(char* input)
{
  DynamicJsonDocument doc(2048);
  DeserializationError error = deserializeJson(doc, input);

  if(doc.containsKey("startupAnimation"))
    startupAnimation = doc["startupAnimation"].as<String>();
  if(doc.containsKey("color"))
    color = doc["color"].as<String>();
  if(doc.containsKey("speed"))
    speed = doc["speed"];
  if(doc.containsKey("saturation"))
    saturation = doc["saturation"];

  return !error;
}

String ConfigClass::getJSON()
{
  DynamicJsonDocument doc(2048);

  IPAddress ip = WiFi.localIP();
  doc["startupAnimation"] = startupAnimation;
  doc["color"] = color;
  doc["speed"] = speed;
  doc["saturation"] = saturation;
  doc["status"] = String(status);
  doc["currentAnimation"] = currentAnimation->getName();
  JsonArray a = doc.createNestedArray("animations");
  for(uint8_t i = 0; i < animations.size(); i++)
  {
    a.add(animations.get(i)->getName());
  }

  String buffer = "";
  serializeJson(doc, buffer);

  return buffer;
}

bool ConfigClass::init()
{
  if (!SPIFFS.begin())
  {
    Serial.println("[Config] Couldn't mount file system.");
    return false;
  }

  File configFile = SPIFFS.open(config_filename, "r");
  if (!configFile)
  {
    Serial.println("[Config] Opening file '" + config_filename + "' failed.");
    return false;
  }

  String content;
  if(configFile.available())
  {
    content = configFile.readString();
  }

  char json[content.length()];
  content.toCharArray(json, content.length());

  return parseJSON(json);
}

bool ConfigClass::save()
{
  File configFile = SPIFFS.open(config_filename, "w");
  if(!configFile)
  {
    Serial.println("[Config] Opening file " + config_filename + " for saving failed.");
    return false;
  }

  configFile.println(getJSON());
  configFile.close();

  return true;
}

ConfigClass Config;