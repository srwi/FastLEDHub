#pragma once

#include <Arduino.h>
#include <WebSocketsServer.h>

namespace WebSocket
{

/// Websocket instance used for communication with web interface
extern WebSocketsServer socket;

/// Initialize the websocket instance
void initialize();

/// Callback function for websocket events
/// @param id Connection id that triggered event
/// @param type Event type
/// @param payload Payload
/// @param length Payload length
void handle(uint8_t id, WStype_t type, uint8_t *payload, size_t length);

/// Handle websocket events of type WStype_TEXT
/// @param text Text
/// @param id Connection id that triggered event
void handleText(String text, uint8_t id);

/// Handle websocket events of type WStype_BIN
/// @param binary Byte array
/// @param id Connection id that triggered event
void handleBinary(uint8_t *binary, uint8_t id);

/// Broadcast the current animation status to all active websocket connections
void broadcastStatus();

/// Convert a byte array to String
/// @param bytes Byte array
/// @return Converted String
String byteArray2string(uint8_t *bytes);

} // namespace Websocket
