#ifndef _WEBSOCKET_H
#define _WEBSOCKET_H

#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include "EffectController.h"
#include "Config.h"

extern WebSocketsServer webSocket;
extern uint8_t websocketConnectionCount;

void initWebsocket();
void websocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void handleWebsocketText(String text, uint8_t num);
void handleWebsocketBinary(uint8_t *binary);
String getEffectSettingsJSON();
String byteArrayToString(uint8_t *bytes);

#endif