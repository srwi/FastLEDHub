#include "WebSocket.h"

#include "Animation.h"
#include "ColorUtils.h"
#include "Config.h"
#include "Fade.h"
#include "FastLEDManager.h"
#include "SerialOut.h"
#include "Spectroscope.h"

#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <Hash.h>


namespace WebSocket
{

WebSocketsServer socket = WebSocketsServer(81);

void initialize()
{
  socket.begin();
  socket.onEvent(handle);
  if (MDNS.begin("lightstrip"))
    PRINTLN_VERBOSE("MDNS responder started");
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
}

void handle(uint8_t id, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    Config.save();
    PRINTF("[%u] Disconnected!\n", id);
    break;
  case WStype_CONNECTED:
  {
    IPAddress ip = socket.remoteIP(id);
    PRINTF("[%u] Connected from %d.%d.%d.%d url: %s\n", id, ip[0], ip[1], ip[2], ip[3], payload);
  }
  break;
  case WStype_TEXT:
    PRINTF_VERBOSE("[%u] Got text: %s\n", id, payload);
    handleText(byteArray2string(payload), id);
    break;
  case WStype_BIN:
    PRINTF_VERBOSE("[%u] Got binary: %s\n", id, payload);
    handleBinary(payload, id);
    break;
  default:
    break;
  }
}

void handleText(String text, uint8_t id)
{
  char textArray[text.length()];
  text.toCharArray(textArray, text.length());

  if (Config.parseJson(textArray))
    Config.save();
}

void handleBinary(uint8_t *binary, uint8_t id)
{
  switch (binary[0])
  {
  case 0: // Color
    Config.color = rgb2hex(binary[1], binary[2], binary[3]);
    FastLEDManager.begin(FastLEDManager.getAnimation("Color"));
    break;
  case 1: // Toggle animation
    Fade::stop();
    FastLEDManager.toggle(FastLEDManager.getAnimation(binary[1]));
    break;
  case 2: // Stop
    FastLEDManager.stop();
    break;
  case 10: // Spectroscope data
    if (FastLEDManager.currentAnimation)
      FastLEDManager.stop();
    for (uint16_t i = 0; i < FastLEDManager.numLeds; i++)
      FastLEDManager.leds[i] = CRGB(binary[1 + i * 3], binary[2 + i * 3], binary[3 + i * 3]);
    break;
  case 11: // linear spectroscope data
    Spectroscope::updateSpectroscope(binary + 1, false);
    break;
  case 12: // Symmetrical spectroscope data
    Spectroscope::updateSpectroscope(binary + 1, true);
    break;
  case 20: // Speed
    Config.speed = binary[1];
    break;
  case 21: // Saturation
    Config.saturation = binary[1];
    break;
  case 30: // Request configuration
    DynamicJsonDocument doc(2048);
    doc = Config.getJson(doc);
    doc["status"] = String(FastLEDManager.status);
    doc["currentAnimation"] = FastLEDManager.currentAnimation ? FastLEDManager.currentAnimation->getName() : "";
    JsonArray animations = doc.createNestedArray("animations");
    for (uint8_t i = 0; i < FastLEDManager.animations.size(); i++)
    {
      animations.add(FastLEDManager.animations.get(i)->getName());
    }
    String buffer = "";
    serializeJson(doc, buffer);
    socket.sendTXT(id, buffer.c_str());
    break;
  }
}

String byteArray2string(uint8_t *bytes)
{
  String s = "";
  for (uint16_t i = 0; bytes[i] != '\0'; i++)
  {
    s += char(bytes[i]);
  }
  s += '\0';
  return s;
}

void broadcastStatus()
{
  String msg = "{\"status\": " + String((int)FastLEDManager.status) + ",\"currentAnimation\":\"" +
    (FastLEDManager.currentAnimation ? FastLEDManager.currentAnimation->getName() : "") + "\"\n}";
  WebSocket::socket.broadcastTXT(msg.c_str());
}

} // namespace WebSocket
