#include "WebSocket.h"

WebSocketsServer webSocket = WebSocketsServer(81);

void websocket_init()
{
	webSocket.begin();
	webSocket.onEvent(websocket_event);
	if(MDNS.begin("lightstrip"))
	{
		Serial.println("[Websocket] MDNS responder started");
	}
	MDNS.addService("http", "tcp", 80);
	MDNS.addService("ws", "tcp", 81);
}

void websocket_event(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
	switch(type)
	{
		case WStype_DISCONNECTED:
			Serial.printf("[Websocket] [%u] Disconnected!\n", num);
		break;
		case WStype_CONNECTED:
		{
			IPAddress ip = webSocket.remoteIP(num);
			Serial.printf("[Websocket] [%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
			webSocket.sendTXT(num, Config.getJSON().c_str());
		}
		break;
		case WStype_TEXT:
			Serial.printf("[Websocket] [%u] get Text: %s\n", num, payload);
			handle_websocket_text(byte_array_to_String(payload), num);
		break;
		case WStype_BIN:
			//Serial.printf("[Websocket] [%u] get Binary: %s\n", num, payload);
			handle_websocket_binary(payload);
		break;
	}
}

void handle_websocket_text(String text, uint8_t num)
{
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
	else if(text == "wakeup")
	{
		begin(wakeup);
	}
	else
	{
		Serial.println("Command '" + text + "' not found!");
	}
}

void handle_websocket_binary(uint8_t *binary)
{
	switch(binary[0])
	{
		case 0: // Color
		{
			begin(staticSingleColor);

			rgb newColor = { byteToColorRange(binary[1]), byteToColorRange(binary[2]), byteToColorRange(binary[3]) };
			staticSingleColorNamespace::currentStaticSingleColor = newColor;
		}
		break;
	}
}

String byte_array_to_String(uint8_t *bytes)
{
	String s = "";
	for(uint16_t i = 0; bytes[i] != '\0'; i++)
	{
		s += char(bytes[i]);
	}
	s += '\0';
	return s;
}