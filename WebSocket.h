#pragma once

#include <Arduino.h>
#include <WebSocketsServer.h>

namespace WebSocket
{

extern WebSocketsServer webSocket;
extern uint8_t websocketConnectionCount;

void initialize();
void handle(uint8_t num, WStype_t type, uint8_t *payload, size_t length);
void handleText(String text, uint8_t num);
void handleBinary(uint8_t *binary, uint8_t num);
void broadcastStatus();
String byteArrayToString(uint8_t *bytes);

} // namespace Websocket
