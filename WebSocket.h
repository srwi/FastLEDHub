#ifndef _WEBSOCKET_H
#define _WEBSOCKET_H

#include <WebSocketsServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

#include "EffectController.h"
#include "Config.h"

#include "Effects/Wakeup.h"
#include "Effects/StaticSingleColor.h"

extern WebSocketsServer webSocket;

void websocket_init();
void websocket_event(uint8_t num, WStype_t type, uint8_t * payload, size_t length);
void handle_websocket_text(String text, uint8_t num);
void handle_websocket_binary(uint8_t *binary);
String byte_array_to_String(uint8_t *bytes);

#endif