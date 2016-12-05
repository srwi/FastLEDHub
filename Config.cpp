#include "Config.h"

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
		content = configFile.readString();

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

bool ConfigClass::parseJSON(char* json)
{
	StaticJsonBuffer<DATA_JSON_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);

	// String example: this_is_a_string = root["this_is_a_string"].asString();
	if(root.containsKey("time_zone"))
		time_zone = root["time_zone"];
	if(root.containsKey("summer_time"))
		summer_time = root["summer_time"];
	if(root.containsKey("sunset_offset"))
		sunset_offset = root["sunset_offset"];
	if(root.containsKey("wakeup_red"))
		wakeup_red = root["wakeup_red"];
	if(root.containsKey("wakeup_green"))
		wakeup_green = root["wakeup_green"];
	if(root.containsKey("wakeup_blue"))
		wakeup_blue = root["wakeup_blue"];
	if(root.containsKey("wakeup_duration"))
		wakeup_duration = root["wakeup_duration"];
	if(root.containsKey("fallback_sunset_hour"))
		fallback_sunset_hour = root["fallback_sunset_hour"];
	if(root.containsKey("fallback_sunset_minute"))
		fallback_sunset_minute = root["fallback_sunset_minute"];
	if(root.containsKey("longitude"))
		longitude = root["longitude"];
	if(root.containsKey("latitude"))
		latitude = root["latitude"];
	
	return root.success();
}

String ConfigClass::getJSON()
{
	StaticJsonBuffer<DATA_JSON_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();

	root["time_zone"] = time_zone;
	root["summer_time"] = summer_time;
	root["sunset_offset"] = sunset_offset;
	root["wakeup_red"] = wakeup_red;
	root["wakeup_green"] = wakeup_green;
	root["wakeup_blue"] = wakeup_blue;
	root["wakeup_duration"] = wakeup_duration;
	root["fallback_sunset_hour"] = fallback_sunset_hour;
	root["fallback_sunset_minute"] = fallback_sunset_minute;
	root["longitude"] = longitude;
	root["latitude"] = latitude;

	String buffer = "";
	root.prettyPrintTo(buffer);

	return buffer;
}

ConfigClass Config;