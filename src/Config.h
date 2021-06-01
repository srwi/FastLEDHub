#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>


class ConfigClass
{
public:
  /// Time zone
  int8_t timeZone = 0;
  /// Summer time (0 = false, 1 = true)
  int8_t summerTime = 0;
  /// Longitude
  float longitude = 0;
  /// Latitude
  float latitude = 0;
  // Alarm functionality enabled
  bool alarmEnabled = false;
  /// Alarm duration in minutes
  uint16_t alarmDuration = 0;
  /// Alarm time hour
  uint8_t alarmHour = 0;
  /// Alarm time minute
  uint8_t alarmMinute = 0;
  /// Animation during alarm duration (while the brightness increases)
  String alarmAnimation = "";
  /// Animation triggered after the alarm duration has ended
  String postAlarmAnimation = "";
  /// Sunset functionality enabled
  bool sunsetEnabled = false;
  /// Sunset duration in minutes
  uint16_t sunsetDuration = 0;
  /// Sunset time hour
  int8_t sunsetHour = 0;
  /// Sunset time minute
  int8_t sunsetMinute = 0;
  /// Sunset time offset in minutes relative to the
  /// automatically obtained local sunset time
  int16_t sunsetOffset = 0;
  /// Sunset animation
  String sunsetAnimation = "";
  /// Startup animation that gets triggered when powering up the device
  String startupAnimation = "";
  /// Lastly used color for Color animation
  String color = "";
  /// Current animation speed
  uint8_t speed = 128;
  /// Current led saturation
  uint8_t saturation = 255;

  /// Initialize config object by mounting file system and
  /// reading the config file.
  /// @return True if successfull
  bool initialize();

  /// Save config to file system
  /// @return True if successfull
  bool save();

  /// Parse a JSON char array and apply its values to the
  /// config state.
  /// @param input JSON char array
  /// @return True if successfull
  bool parseJson(char *input);

  /// Get config state contained in a DynamicJsonDocument
  /// @return DynamicJsonDocument containing config
  DynamicJsonDocument getJson(DynamicJsonDocument doc);

  /// Get config state serialized as a JSON string
  /// @return JSON string
  String getJsonString();

private:
  /// Constant config filename
  const String configFilename = "/config.txt";
};

extern ConfigClass Config;
