#include "WebSocket.h"

WebSocketsServer webSocket = WebSocketsServer(81);
uint8_t websocketConnectionCount = 0;

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
			Serial.printf("[Websocket] [%u] Disconnected!\n", num);
		break;
		case WStype_CONNECTED:
		{
			websocketConnectionCount++;
			IPAddress ip = webSocket.remoteIP(num);
			Serial.printf("[Websocket] [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
			webSocket.sendTXT(num, Config.getJSON().c_str());
		}
		break;
		case WStype_TEXT:
			Serial.printf("[Websocket] [%u] get Text: %s\n", num, payload);
			handleWebsocketText(byteArrayToString(payload), num);
		break;
		case WStype_BIN:
			//Serial.printf("[Websocket] [%u] get Binary: %s\n", num, payload);
			handleWebsocketBinary(payload);
		break;
	}
}

void handleWebsocketText(String text, uint8_t num)
{
	// TODO: use .c_str()
	char textArray[text.length()];
	text.toCharArray(textArray, text.length());

	if(Config.parseJSON(textArray))
	{
		Config.save();
		return;
	}

	if(text == "restart")
	{
		ESP.restart();
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
		case 0: // Color
		{
			customColorNamespace::set(CRGB(binary[1], binary[2], binary[3]));
			begin("Custom Color");
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