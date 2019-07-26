#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPEssentials.h>
#include <ESP8266WiFi.h>

class ConfigClass
{
  public:
    // other
    String startup_effect = "";
    String custom_color = "";
    String custom_color2 = "";
    String current_effect = "";
    float speed = 0;
    uint8_t saturation = 255;

    bool init();
    bool save();
    bool parseJSON(char* input);
    String getJSON();

  private:
    const String config_filename = "/config_json.txt";
};

extern ConfigClass Config;