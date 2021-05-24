#include "WebSocket.h"

#include "Animation.h"
#include "ColorUtils.h"
#include "Config.h"
#include "Fade.h"
#include "FastLEDManager.h"
#include "Spectroscope.h"

#include <Hash.h>
#include <ESP8266mDNS.h>
#include <ArduinoJson.h>


namespace WebSocket
{

WebSocketsServer webSocket = WebSocketsServer(81);
uint8_t websocketConnectionCount = 0;
bool liveDataHasChanged = false;

void initialize()
{
  webSocket.begin();
  webSocket.onEvent(handle);
  if (MDNS.begin("lightstrip"))
    Serial.println("MDNS responder started");
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
}

void handle(uint8_t num, WStype_t type, uint8_t *payload, size_t length)
{
  switch (type)
  {
  case WStype_DISCONNECTED:
    // Save config if color, speed or saturation changed
    if (liveDataHasChanged)
    {
      FastLEDManager.config->save();
      liveDataHasChanged = false;
    }
    websocketConnectionCount--;
    Serial.printf("[%u] Disconnected!\n", num);
    break;
  case WStype_CONNECTED:
  {
    websocketConnectionCount++;
    IPAddress ip = webSocket.remoteIP(num);
    Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
  }
  break;
  case WStype_TEXT:
    // Serial.printf("[%u] Got text: %s\n", num, payload);
    handleText(byteArrayToString(payload), num);
    break;
  case WStype_BIN:
    // Serial.printf("[%u] Got binary: %s\n", num, payload);
    handleBinary(payload, num);
    break;
  // Suppress warnings:
  case WStype_ERROR:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_FIN:
  case WStype_PONG:
  case WStype_PING:
    break;
  }
}

void handleText(String text, uint8_t num)
{
  char textArray[text.length()];
  text.toCharArray(textArray, text.length());

  if (FastLEDManager.config->parseJSON(textArray))
  {
    FastLEDManager.config->save();
    return;
  }

  if (text.startsWith("toggle"))
  {
    String animation = text.substring(7);
    Fade::stop();
    FastLEDManager.toggle();
  }
  else if (text == "stop")
  {
    if (FastLEDManager.currentAnimation)
      FastLEDManager.stop();
  }
  else if (text == "requesting_config")
  {
    webSocket.sendTXT(num, FastLEDManager.config->getJSON().c_str());
    // TODO: Also send animations list, status and current animation
    // doc["status"] = String(FastLEDManager.status);
    // doc["currentAnimation"] = FastLEDManager.currentAnimation ? FastLEDManager.currentAnimation->getName() : "";
    // JsonArray animations = doc.createNestedArray("animations");
    // for (uint8_t i = 0; i < FastLEDManager.animations.size(); i++)
    // {
    //   animations.add(FastLEDManager.animations.get(i)->getName());
    // }
  }
  else
  {
    Serial.println("Command '" + text + "' not found!");
  }
}

void handleBinary(uint8_t *binary, uint8_t num)
{
  switch (binary[0])
  {
  case 0: // Color
    FastLEDManager.config->color = rgb2hex(binary[1], binary[2], binary[3]);
    liveDataHasChanged = true;
    FastLEDManager.begin(FastLEDManager.getAnimation("Color"));
    webSocket.sendTXT(num, String("ok").c_str());
    break;
  case 1: // Speed
    FastLEDManager.config->speed = binary[1];
    liveDataHasChanged = true;
    webSocket.sendTXT(num, String("ok").c_str());
    break;
  case 4: // Spectroscope data
    if (FastLEDManager.currentAnimation)
      FastLEDManager.stop();
    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
      // leds[i] = CRGB(binary[1 + i * 3], binary[2 + i * 3], binary[3 + i * 3]);
    }
    break;
  case 5: // Saturation
    FastLEDManager.config->saturation = binary[1];
    liveDataHasChanged = true;
    webSocket.sendTXT(num, String("ok").c_str());
    break;
  case 6: // linearSpectroscope data
    Spectroscope::linearSpectroscope(binary + 1);
    break;
  case 7: // symmetricalSpectroscope data
    Spectroscope::symmetricalSpectroscope(binary + 1);
    break;
  }
}

String byteArrayToString(uint8_t *bytes)
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
  // Send status as JSON
  String msg = "{\n  \"status\": " + String((int)FastLEDManager.status) + ",\n  \"currentAnimation\": \"" + (FastLEDManager.currentAnimation ? FastLEDManager.currentAnimation->getName() : "") + "\"\n}";
  WebSocket::webSocket.broadcastTXT(msg.c_str());
}

} // namespace WebSocket
