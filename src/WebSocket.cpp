#include "WebSocket.h"

#include <ArduinoJson.h>
#if defined(ESP32)
  #include <ESPmDNS.h>
#elif defined(ESP8266)
  #include <ESP8266mDNS.h>
#endif
#define FASTLED_INTERNAL
#include <FastLED.h>

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

    WebSocketsServer m_socket = WebSocketsServer(81);

    /// Handle websocket events of type WStype_TEXT
    /// @param text Text String
    /// @param id Connection id that triggered event
    void onText(uint8_t *text, uint8_t id)
    {
      if (Config.parseJson((const char *)text))
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
      {
        CRGB color = CRGB(binary[2], binary[3], binary[4]);
        Config.colorPickerValues.set(binary[1], color);
        FastLEDHub.getColorPicker(binary[1])->value = color;
        break;
      }
      case 1: // Toggle animation
        Fade::stop();
        FastLEDHub.toggle(FastLEDHub.getAnimation(binary[1]));
        break;
      case 2: // Stop
        FastLEDHub.stop();
        break;
      case 10: // Led data
        FastLEDHub.stop();
        Fade::stop();
        for (uint16_t i = 0; i < FastLEDHub.size(); ++i)
          FastLEDHub.leds()[i] = CRGB(binary[1 + i * 3], binary[2 + i * 3], binary[3 + i * 3]);
        break;
      case 20: // Slider data
      {
        int16_t value = (binary[2] << 8) | binary[3];
        if (binary[1] == 0)
          FastLEDHub.setBrightness(value);
        else if (binary[1] == 1)
          FastLEDHub.setSpeed(value);
        Config.sliderValues.set(binary[1], value);
        FastLEDHub.getSlider(binary[1])->value = value;
        break;
      }
      case 30: // Spectrum data
        for (uint16_t i = 0; i < SPECTRUM_LENGTH; ++i)
        {
          FastLEDHub.spectrumData[i] = binary[i + 1];
        }
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
        IPAddress ip = m_socket.remoteIP(id);
        PRINTF("[%u] Connected from %d.%d.%d.%d url: %s\n", id, ip[0], ip[1], ip[2], ip[3], payload);
        String config = Config.asString(true);
        WebSocket::m_socket.sendTXT(id, config.c_str());
      }
      break;
      case WStype_TEXT:
        PRINTF_VERBOSE("[%u] Got text: %s\n", id, payload);
        onText(payload, id);
        break;
      case WStype_BIN:
        PRINTF_VERBOSE("[%u] Got binary: %s\n", id, payload);
        onBinary((uint8_t *)payload, id);
        break;
      default:
        break;
      }
    }

  } // namespace

  void initialize(const String &hostname)
  {
    m_socket.begin();
    m_socket.onEvent(onEvent);
    MDNS.begin(hostname);
    MDNS.addService("http", "tcp", 80);
    MDNS.addService("ws", "tcp", 81);
  }

  void handle()
  {
    m_socket.loop();
  }

  void broadcastMessage(String msg)
  {
    WebSocket::m_socket.broadcastTXT(msg.c_str());
  }

  void broadcastStatus()
  {
    String msg = "{\"status\": " + String((int)FastLEDHub.getStatus()) + ", \"currentAnimation\": \"" + FastLEDHub.getCurrentAnimationName() + "\"}";
    broadcastMessage(msg);
  }

} // namespace WebSocket
