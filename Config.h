#ifndef _CONFIG_H
#define _CONFIG_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESPEssentials.h>

#define DATA_JSON_SIZE (JSON_OBJECT_SIZE(11))

class ConfigClass
{
	public:
		int8_t time_zone = 0;
		int8_t summer_time = 0;
		int16_t sunset_offset = 0;
		uint16_t wakeup_red = 0;
		uint16_t wakeup_green = 0;
		uint16_t wakeup_blue = 0;
		uint16_t wakeup_duration = 0;
		uint8_t fallback_sunset_hour = 0;
		uint8_t fallback_sunset_minute = 0;
		float longitude = 0;
		float latitude = 0;

		bool init();
		bool save();
		bool parseJSON(char* json);
		String getJSON();

	private:
		const String config_filename = "/config_json.txt";
};

extern ConfigClass Config;

#endif