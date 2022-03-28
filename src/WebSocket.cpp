#include "WebSocket.h"

#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <Hash.h>

#include "Animation.h"
#include "ColorUtils.h"
#include "Config.h"
#include "Fade.h"
#include "FastLEDHub.h"
#include "SerialOut.h"

namespace WebSocket
{

  namespace
  {

    WebSocketsServer socket = WebSocketsServer(81);

    /// Convert a byte array to String
    /// @param bytes Byte array
    /// @return Converted String
    String byteArray2String(uint8_t *bytes)
    {
      String s = "";
      for (uint16_t i = 0; bytes[i] != '\0'; i++)
      {
        s += char(bytes[i]);
      }
      s += '\0';
      return s;
    }

    /// Handle websocket events of type WStype_TEXT
    /// @param text Text
    /// @param id Connection id that triggered event
    void onText(String text, uint8_t id)
    {
      if (Config.parseJson(text.c_str()))
        Config.save();
    }

    /// Handle websocket events of type WStype_BIN
    /// @param binary Byte array
    /// @param id Connection id that triggered event
    void onBinary(uint8_t *binary, uint8_t id)
    {
      switch (binary[0])
      {
      case 0: // Color
        Config.color = rgb2hex(binary[1], binary[2], binary[3]);
        FastLEDHub.begin(FastLEDHub.getAnimation("Color"));
        break;
      case 1: // Toggle animation
        Fade::stop();
        FastLEDHub.toggle(FastLEDHub.getAnimation(binary[1]));
        break;
      case 2: // Stop
        FastLEDHub.stop();
        break;
      case 10: // Color data
        FastLEDHub.stop();
        Fade::stop();
        for (uint16_t i = 0; i < FastLEDHub.size(); i++)
          FastLEDHub.leds()[i] =
              CRGB(binary[1 + i * 3], binary[2 + i * 3], binary[3 + i * 3]);
        break;
      case 20: // Slider data
      {
        int16_t value = (binary[2] << 8) | binary[3];
        if (binary[1] == 0)
          FastLEDHub.brightness10 = value;
        else if (binary[1] == 1)
          FastLEDHub.speed = value;
        Config.sliderValues.set(binary[1], value);
        FastLEDHub.sliders.get(binary[1])->value = value;
        break;
      }
      case 30: // Request configuration
        String config = Config.asString(true);
        WebSocket::socket.sendTXT(id, config.c_str());
        break;
      }
    }

    /// Callback function for websocket events
    /// @param id Connection id that triggered event
    /// @param type Event type
    /// @param payload Payload
    /// @param length Payload length
    void onEvent(uint8_t id, WStype_t type, uint8_t *payload, size_t length)
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
        PRINTF("[%u] Connected from %d.%d.%d.%d url: %s\n", id, ip[0], ip[1],
               ip[2], ip[3], payload);
      }
      break;
      case WStype_TEXT:
        PRINTF_VERBOSE("[%u] Got text: %s\n", id, payload);
        onText(byteArray2String(payload), id);
        break;
      case WStype_BIN:
        PRINTF_VERBOSE("[%u] Got binary: %s\n", id, payload);
        onBinary(payload, id);
        break;
      default:
        break;
      }
    }

  } // namespace

  void initialize()
  {
    socket.begin();
    socket.onEvent(onEvent);
    MDNS.begin("fastledhub");
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);
  }

  void handle()
  {
    socket.loop();
  }

  void broadcastStatus()
  {
    String currentAnimation = (FastLEDHub.currentAnimation ? FastLEDHub.currentAnimation->getName() : "");
    String msg = "{\"status\": " + String((int)FastLEDHub.status) + ",\"currentAnimation\":\"" + currentAnimation + "\"\n}";
    WebSocket::socket.broadcastTXT(msg.c_str());
  }

} // namespace WebSocket
