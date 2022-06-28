#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <LinkedList.h>

class ConfigClass
{
public:
  int8_t timeZone = 0;
  int8_t summerTime = 0;
  float longitude = 0;
  float latitude = 0;
  bool alarmEnabled = false;
  uint16_t alarmDuration = 1;
  uint8_t alarmHour = 0;
  uint8_t alarmMinute = 0;
  String alarmAnimation = "";
  String postAlarmAnimation = "";
  bool sunsetEnabled = false;
  uint16_t sunsetDuration = 1;
  int8_t sunsetHour = 0;
  int8_t sunsetMinute = 0;
  int16_t sunsetOffset = 0;
  String sunsetAnimation = "";
  String startupAnimation = "";
  LinkedList<int16_t> sliderValues;
  LinkedList<CRGB> colorPickerValues;

  /// Initialize config object by reading the config file.
  /// @return True if successful
  bool initialize();

  /// Save config to file system
  /// @return True if successful
  bool save();

  /// Parse a JSON char array and apply its values to the config.
  /// @param input JSON char array
  /// @return True if successful
  bool parseJson(const char *input);

  /// Get config and (optional) application state as JSON String.
  /// @param includeApplicationState Include application state (default: false)
  /// @return JSON String
  String asString(bool includeApplicationState = false);

private:
  const String m_configFilename = "/config.txt";

  void getUserConfigJson(JsonDocument &doc);
  void getApplicationStateJson(JsonDocument &doc);
};

extern ConfigClass Config;
