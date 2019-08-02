#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPEssentials.h>
#include <ESP8266WiFi.h>

#include "Animation.h"

class ConfigClass
{
  public:
    String startupAnimation = "";
    String color = "";
    uint8_t speed = 128;
    uint8_t saturation = 255;

    bool init();
    bool save();
    bool parseJSON(char* input);
    String getJSON();

  private:
    const String config_filename = "/config_json.txt";
};

extern ConfigClass Config;