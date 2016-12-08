#include "Config.h"

bool ConfigClass::parseJSON(char* json)
{
	StaticJsonBuffer<DATA_JSON_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);

	// time specific
	if(root.containsKey("time_zone"))
		time_zone = root["time_zone"];
	if(root.containsKey("summer_time"))
		summer_time = root["summer_time"];
	if(root.containsKey("longitude"))
		longitude = root["longitude"];
	if(root.containsKey("latitude"))
		latitude = root["latitude"];
	// alarm
	if(root.containsKey("alarm_duration"))
		alarm_duration = root["alarm_duration"];
	if(root.containsKey("alarm_hour"))
		alarm_hour = root["alarm_hour"];
	if(root.containsKey("alarm_minute"))
		alarm_minute = root["alarm_minute"];
	// sunset
	if(root.containsKey("sunset_hour"))
		sunset_hour = root["sunset_hour"];
	if(root.containsKey("sunset_minute"))
		sunset_minute = root["sunset_minute"];
	if(root.containsKey("sunset_duration"))
		sunset_duration = root["sunset_duration"];
	if(root.containsKey("sunset_offset"))
		sunset_offset = root["sunset_offset"];
	// other
	if(root.containsKey("last_effect"))
		last_effect = root["last_effect"].asString();
	
	return root.success();
}

String ConfigClass::getJSON()
{
	StaticJsonBuffer<DATA_JSON_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();

	// time specific
	root["time_zone"] = time_zone;
	root["summer_time"] = summer_time;
	root["longitude"] = longitude;
	root["latitude"] = latitude;
	// alarm
	root["alarm_hour"] = alarm_hour;
	root["alarm_minute"] = alarm_minute;
	root["alarm_duration"] = alarm_duration;
	// sunset
	root["sunset_hour"] = sunset_hour;
	root["sunset_minute"] = sunset_minute;
	root["sunset_duration"] = sunset_duration;
	root["sunset_offset"] = sunset_offset;
	// other
	root["last_effect"] = last_effect;

	String buffer = "";
	root.prettyPrintTo(buffer);

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

ConfigClass Config;