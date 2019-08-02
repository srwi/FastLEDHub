#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPEssentials.h>
#include <ESP8266WiFi.h>

#include "Animation.h"

class ConfigClass
{
  public:
    // Time
    int8_t timeZone = 0;
    int8_t summerTime = 0;
    float longitude = 0;
    float latitude = 0;
    // Alarm
    bool alarmEnabled = false;
    uint16_t alarmDuration = 0;
    uint8_t alarmHour = 0;
    uint8_t alarmMinute = 0;
    String alarmAnimation = "";
    String postAlarmAnimation = "";
    // Sunset
    bool sunsetEnabled = false;
    uint16_t sunsetDuration = 0;
    uint8_t sunsetHour = 0;
    uint8_t sunsetMinute = 0;
    int16_t sunsetOffset = 0;
    String sunsetAnimation = "";
    // Other properties
    String startupAnimation = "";
    String color = "";
    uint8_t speed = 128;
    uint8_t saturation = 255;

    bool init();
    bool save();
    bool parseJSON(char* input);
    String getJSON();

  private:
    const String configFilename = "/config_json.txt";
};

extern ConfigClass Config;