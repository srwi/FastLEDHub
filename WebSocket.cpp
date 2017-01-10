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
			// Save config if live data (custom color or speed) has changed within websocket connection
			if(liveDataHasChanged)
			{
				Config.save();
				liveDataHasChanged = false;
			}
			websocketConnectionCount--;
			Serial.printf("[Websocket] [%u] Disconnected!\n", num);
		break;
		case WStype_CONNECTED:
		{
			websocketConnectionCount++;
			IPAddress ip = webSocket.remoteIP(num);
			Serial.printf("[Websocket] [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

			// Send information to websocket client
			// TODO: Send these as one message
			webSocket.sendTXT(num, Config.getJSON().c_str());
			webSocket.sendTXT(num, getEffectSettingsJSON().c_str());
			broadcastStatus();
			webSocket.sendTXT(num, "ok");
		}
		break;
		case WStype_TEXT:
			//Serial.printf("[Websocket] [%u] get Text: %s\n", num, payload);
			handleWebsocketText(byteArrayToString(payload), num);
		break;
		case WStype_BIN:
			//Serial.printf("[Websocket] [%u] get Binary: %s\n", num, payload);
			handleWebsocketBinary(payload, num);
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
		Config.save();
		return;
	}

	if(text.startsWith("toggle"))
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

void handleWebsocketBinary(uint8_t *binary, uint8_t num)
{
	switch(binary[0])
	{
		case 0: // Custom Color
			customColorNamespace::set(CRGB(binary[1], binary[2], binary[3]));
			Config.custom_color = String(binary[1], HEX) + String(binary[2], HEX) + String(binary[3], HEX);
			begin("Farbe");

			liveDataHasChanged = true;
			webSocket.sendTXT(num, String("ok").c_str());
		break;
		case 1: // Speed
			setSpeed(binary[1]);

			liveDataHasChanged = true;
			webSocket.sendTXT(num, String("ok").c_str());
		break;
		case 2: // Set alarm
			Config.alarm_hour = binary[1];
			Config.alarm_minute = binary[2];
			Config.alarm_enabled = true;
			Config.save();
			webSocket.sendTXT(num, String("alarm set").c_str());
		break;
		case 3: // Disable alarm
			Config.alarm_enabled = false;
			Config.save();
			webSocket.sendTXT(num, String("alarm disabled").c_str());
		break;
		case 4: // Spectroscope data
			if(status != STOPPED)
				stop();
			for(uint16_t i = 0; i < NUM_LEDS; i++)
			{
				strip[i] = CRGB(binary[1 + i*3], binary[2 + i*3], binary[3 + i*3]);
			}
		break;
	}
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