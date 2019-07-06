#include "Config.h"

bool ConfigClass::parseJSON(char* input)
{
	DynamicJsonDocument doc(2048);
	DeserializationError error = deserializeJson(doc, input);

	if(doc.containsKey("startup_effect"))
		startup_effect = doc["startup_effect"].as<String>();
	if(doc.containsKey("custom_color"))
		custom_color = doc["custom_color"].as<String>();
	if(doc.containsKey("custom_color2"))
		custom_color2 = doc["custom_color2"].as<String>();
	if(doc.containsKey("speed"))
		speed = doc["speed"];
	if(doc.containsKey("saturation"))
		saturation = doc["saturation"];
	if(doc.containsKey("current_effect"))
		current_effect = doc["current_effect"].as<String>();

	return !error;
}

String ConfigClass::getJSON()
{
	DynamicJsonDocument doc(2048);

	IPAddress ip = WiFi.localIP();
	doc["own_ip"] = String(String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]));
	doc["startup_effect"] = startup_effect;
	doc["custom_color"] = custom_color;
	doc["custom_color2"] = custom_color2;
	doc["speed"] = speed;
	doc["saturation"] = saturation;
	// TODO: root["status"] = String(status);
	// TODO: root["current_effect"] = effectList.get(effectIndex).name;
	// animations list
	// JsonArray animations = doc.createNestedArray("animations");
	// for(uint8_t i = 0; i < effectList.size(); i++)
	// {
	// 	animations.add(effectList.get(i).name);
	// }

	String buffer = "";
	serializeJson(doc, buffer);

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
	{
		content = configFile.readString();
	}

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