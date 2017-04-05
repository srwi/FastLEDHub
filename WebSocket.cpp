#include "WebSocket.h"

WebSocketsServer webSocket = WebSocketsServer(81);
uint8_t websocketConnectionCount = 0;
bool liveDataHasChanged = false;
String ipString;

void initWebsocket()
{
	webSocket.begin();
	webSocket.onEvent(websocketEvent);
	if(MDNS.begin("lightstrip"))
	{
		Serial.println("[Wifi] MDNS responder started");
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
			ipString = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
			Serial.printf("[Websocket] [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
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
	else if(text == "mobile")
	{
		webSocket.sendTXT(num, Config.getJSON().c_str());
		if(Config.mobile_ip != ipString)
		{
			Config.mobile_ip = ipString;
			Config.save();
			Serial.println("Got new mobile IP: " + ipString);
		}
	}
	else if(text == "desktop")
	{
		webSocket.sendTXT(num, Config.getJSON().c_str());
		if(Config.desktop_ip != ipString)
		{
			Config.desktop_ip = ipString;
			Config.save();
			Serial.println("Got new desktop IP: " + ipString);
		}
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
			begin(customColor);
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
		case 5: // Saturation
			Config.saturation = binary[1];
			liveDataHasChanged = true;
			webSocket.sendTXT(num, String("ok").c_str());
		break;
		case 6: // linearSpectroscope data
			linearSpectroscope(binary+1);
		break;
		case 7: // symmetricalSpectroscope data
			symmetricalSpectroscope(binary+1);
		break;
		case 8: // Custom Color 2
			customColor2Namespace::set(CRGB(binary[1], binary[2], binary[3]));
			Config.custom_color2 = String(binary[1], HEX) + String(binary[2], HEX) + String(binary[3], HEX);
			begin(customColor2);
			liveDataHasChanged = true;
			webSocket.sendTXT(num, String("ok").c_str());
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

void broadcastStatus()
{
	// Send status as JSON
	String msg = "{\n  \"status\": " + String((int)status) + ",\n  \"current_effect\": \"" + effectList.get(effectIndex).name + "\"\n}";
	webSocket.broadcastTXT(msg.c_str());
}