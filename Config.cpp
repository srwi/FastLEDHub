#include "Config.h"

bool ConfigClass::parseJSON(char* json)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(json);

	if(root.containsKey("startup_effect"))
		startup_effect = root["startup_effect"].asString();
	if(root.containsKey("custom_color"))
		custom_color = root["custom_color"].asString();
	if(root.containsKey("custom_color2"))
		custom_color2 = root["custom_color2"].asString();
	if(root.containsKey("speed"))
		speed = root["speed"];
	if(root.containsKey("saturation"))
		saturation = root["saturation"];
	if(root.containsKey("current_effect"))
		current_effect = root["current_effect"].asString();
	
	return root.success();
}

String ConfigClass::getJSON()
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.createObject();

	IPAddress ip = WiFi.localIP();
	root["own_ip"] = String(String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]));
	root["startup_effect"] = startup_effect;
	root["custom_color"] = custom_color;
	root["custom_color2"] = custom_color2;
	root["speed"] = speed;
	root["saturation"] = saturation;
	root["status"] = String(status);
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