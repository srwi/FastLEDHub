#pragma once

#include <Arduino.h>
#include <Hash.h>
#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>

#include "Config.h"
#include "Spectroscope.h"

extern WebSocketsServer webSocket;
extern uint8_t websocketConnectionCount;

void initWebsocket();
void websocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
String rgbToHex(uint8_t r, uint8_t g, uint8_t b);
void handleWebsocketText(String text, uint8_t num);
void handleWebsocketBinary(uint8_t *binary, uint8_t num);
void broadcastStatus();
String byteArrayToString(uint8_t *bytes);