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
	if(root.containsKey("alarm_enabled"))
		alarm_enabled = root["alarm_enabled"];
	if(root.containsKey("alarm_duration"))
		alarm_duration = root["alarm_duration"];
	if(root.containsKey("alarm_hour"))
		alarm_hour = root["alarm_hour"];
	if(root.containsKey("alarm_minute"))
		alarm_minute = root["alarm_minute"];
	if(root.containsKey("alarm_effect"))
		alarm_effect = root["alarm_effect"].asString();
	if(root.containsKey("post_alarm_effect"))
		post_alarm_effect = root["post_alarm_effect"].asString();
	// sunset
	if(root.containsKey("sunset_enabled"))
		sunset_enabled = root["sunset_enabled"];
	if(root.containsKey("sunset_hour"))
		sunset_hour = root["sunset_hour"];
	if(root.containsKey("sunset_minute"))
		sunset_minute = root["sunset_minute"];
	if(root.containsKey("sunset_duration"))
		sunset_duration = root["sunset_duration"];
	if(root.containsKey("sunset_offset"))
		sunset_offset = root["sunset_offset"];
	if(root.containsKey("sunset_effect"))
		sunset_effect = root["sunset_effect"].asString();
	// other
	if(root.containsKey("custom_color"))
		custom_color = root["custom_color"].asString();
	if(root.containsKey("desktop_ip"))
		desktop_ip = root["desktop_ip"].asString();
	if(root.containsKey("mobile_ip"))
		mobile_ip = root["mobile_ip"].asString();
	if(root.containsKey("speed"))
		speed = root["speed"];
	if(root.containsKey("saturation"))
		saturation = root["saturation"];
	if(root.containsKey("status"))
		speed = root["speed"];
	if(root.containsKey("current_effect"))
		current_effect = root["current_effect"].asString();
	
	return root.success();
}

String ConfigClass::getJSON(bool includeInfo)
{
	StaticJsonBuffer<DATA_JSON_SIZE> jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();

	// time specific
	root["time_zone"] = time_zone;
	root["summer_time"] = summer_time;
	root["longitude"] = longitude;
	root["latitude"] = latitude;
	// alarm
	root["alarm_enabled"] = alarm_enabled;
	root["alarm_hour"] = alarm_hour;
	root["alarm_minute"] = alarm_minute;
	root["alarm_duration"] = alarm_duration;
	root["alarm_effect"] = alarm_effect;
	root["post_alarm_effect"] = post_alarm_effect;
	// sunset
	root["sunset_enabled"] = sunset_enabled;
	root["sunset_hour"] = sunset_hour;
	root["sunset_minute"] = sunset_minute;
	root["sunset_duration"] = sunset_duration;
	root["sunset_offset"] = sunset_offset;
	root["sunset_effect"] = sunset_effect;
	// other
	root["custom_color"] = custom_color;
	root["desktop_ip"] = desktop_ip;
	root["mobile_ip"] = mobile_ip;
	root["speed"] = speed;
	root["saturation"] = saturation;
	root["status"] = (int)status;
	root["current_effect"] = effectList.get(effectIndex).name;
	// effect list
	JsonArray& data = root.createNestedArray("effect_list");
	for(uint8_t i = 0; i < effectList.size(); i++)
	{
		data.add(effectList.get(i).name);
	}

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