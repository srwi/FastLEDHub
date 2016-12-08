#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPEssentials.h>

#define DATA_JSON_SIZE (JSON_OBJECT_SIZE(12))

class ConfigClass
{
	public:
		// time specific
		int8_t time_zone = 0;
		int8_t summer_time = 0;
		float longitude = 0;
		float latitude = 0;
		// alarm
		uint16_t alarm_duration = 0;
		uint8_t alarm_hour = 0;
		uint8_t alarm_minute = 0;
		// sunset
		uint16_t sunset_duration = 0;
		uint8_t sunset_hour = 0;
		uint8_t sunset_minute = 0;
		int16_t sunset_offset = 0;
		// other
		String last_effect = "";

		bool init();
		bool save();
		bool parseJSON(char* json);
		String getJSON();

	private:
		const String config_filename = "/config_json.txt";
};

extern ConfigClass Config;

#endif