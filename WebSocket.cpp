#include "WebSocket.h"

WebSocketsServer webSocket = WebSocketsServer(81);
uint8_t websocketConnectionCount = 0;
bool liveDataHasChanged = false;

void initWebsocket()
{
	webSocket.begin();
	webSocket.onEvent(websocketEvent);
	if(MDNS.begin("lightstrip"))
	{
		Serial.println("[Websocket] MDNS responder started");
	}
	MDNS.addService("http", "tcp", 80);
	MDNS.addService("ws", "tcp", 81);
}

void websocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
	switch(type)
	{
		case WStype_DISCONNECTED:
			websocketConnectionCount--;
			// Save config if live data (custom color or speed) has changed within websocket connection
			if(liveDataHasChanged)
			{
				Config.save();
				liveDataHasChanged = false;
			}
			Serial.printf("[Websocket] [%u] Disconnected!\n", num);
		break;
		case WStype_CONNECTED:
		{
			websocketConnectionCount++;
			IPAddress ip = webSocket.remoteIP(num);
			Serial.printf("[Websocket] [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

			// Send information to websocket client
			webSocket.sendTXT(num, Config.getJSON().c_str());
			webSocket.sendTXT(num, getEffectSettingsJSON().c_str());
			broadcastStatus();
		}
		break;
		case WStype_TEXT:
			//Serial.printf("[Websocket] [%u] get Text: %s\n", num, payload);
			handleWebsocketText(byteArrayToString(payload), num);
		break;
		case WStype_BIN:
			//Serial.printf("[Websocket] [%u] get Binary: %s\n", num, payload);
			handleWebsocketBinary(payload);
		break;
	}
}

String getEffectSettingsJSON()
{
	// TODO: maybe use dynamic buffer?
	StaticJsonBuffer<JSON_OBJECT_SIZE(1) + JSON_ARRAY_SIZE(255)> jsonBuffer;

	JsonObject& root = jsonBuffer.createObject();

	root["alarm_effect"] = Config.alarm_effect;
	root["post_alarm_effect"] = Config.post_alarm_effect;
	root["sunset_effect"] = Config.sunset_effect;

	JsonArray& data = root.createNestedArray("effectList");
	for(uint8_t i = 0; i < effectList.size(); i++)
	{
		data.add(effectList.get(i).name);
	}

	String output = "";
	root.prettyPrintTo(output);

	return output;
}

void handleWebsocketText(String text, uint8_t num)
{
	char textArray[text.length()];
	text.toCharArray(textArray, text.length());

	if(Config.parseJSON(textArray))
	{
		// TODO: this should actually come directly from the web ui itself
		Config.last_effect = effectList.get(effectIndex).name;
		Config.save();
		return;
	}

	if(text == "reboot")
	{
		ESP.restart();
	}
	else if(text == "stop")
	{
		stopFade();
		stop();
	}
	else if(text == "alarm")
	{
		startFade(ALARM);
	}
	else if(text == "sunset")
	{
		startFade(SUNSET);
	}
	else if(text.startsWith("toggle"))
	{
		String effectName = text.substring(7);

		stopFade();
		toggle(effectName);
	}
	else
	{
		Serial.println("Command '" + text + "' not found!");
	}
}

void handleWebsocketBinary(uint8_t *binary)
{
	switch(binary[0])
	{
		case 0: // Custom Color
			customColorNamespace::set(CRGB(binary[1], binary[2], binary[3]));
			Config.custom_color = String(binary[1], HEX) + String(binary[2], HEX) + String(binary[3], HEX);
			begin("Custom Color");
		break;
		case 1: // Speed
			setSpeed(binary[1]);
		break;
	}

	liveDataHasChanged = true;
	webSocket.broadcastTXT(String("pong").c_str());
}

String byteArrayToString(uint8_t *bytes)
{
	String s = "";
	for(uint16_t i = 0; bytes[i] != '\0'; i++)
	{
		s += char(bytes[i]);
	}
	s += '\0';
	return s;
}