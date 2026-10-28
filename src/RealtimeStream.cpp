#include "RealtimeStream.h"
#include "FastLEDHub.h"
#include "SerialOut.h"

#include <WiFiUdp.h>

#if defined(ESP8266)
#include <ESP8266WiFi.h>
#elif defined(ESP32)
#include <WiFi.h>
#endif

namespace RealtimeStream
{
  static WiFiUDP m_udpRealtime;
  static WiFiUDP m_udpDDP;
  static WiFiUDP m_udpHyperion;
  static bool m_udpInitialized = false;

  static uint8_t m_packetBuffer[1500];
  static bool m_active = false;
  static unsigned long m_timeout = 0;
  static uint32_t m_timeoutMs = 2500;

  static void activate(uint32_t timeoutMs)
  {
    if (!m_active)
    {
      m_active = true;
      PRINTLN_VERBOSE("[RealtimeStream] Live streaming active");
    }

    m_timeoutMs = timeoutMs;
    if (timeoutMs == UINT32_MAX)
    {
      m_timeout = UINT32_MAX;
    }
    else
    {
      m_timeout = millis() + timeoutMs;
    }
  }

  static void parseWLEDPacket(const uint8_t *buf, size_t size)
  {
    if (size < 2)
      return;

    uint8_t protocol = buf[0];
    if (protocol < 1 || protocol > 4)
      return;

    uint8_t timeoutSec = buf[1];
    uint32_t timeoutMs = 2500;
    if (timeoutSec == 255)
    {
      timeoutMs = UINT32_MAX;
    }
    else if (timeoutSec > 0)
    {
      timeoutMs = (uint32_t)timeoutSec * 1000;
    }

    activate(timeoutMs);

    CRGB *leds = FastLEDHub.leds();
    int numLeds = FastLEDHub.size();
    if (!leds || numLeds <= 0)
      return;

    if (protocol == 1) // WARLS: [index, r, g, b, ...]
    {
      for (size_t i = 2; i + 3 < size; i += 4)
      {
        uint8_t index = buf[i];
        if (index < numLeds)
        {
          leds[index] = CRGB(buf[i + 1], buf[i + 2], buf[i + 3]);
        }
      }
      FastLEDHub.show();
    }
    else if (protocol == 2) // DRGB: [r, g, b, r, g, b, ...]
    {
      int index = 0;
      for (size_t i = 2; i + 2 < size && index < numLeds; i += 3, index++)
      {
        leds[index] = CRGB(buf[i], buf[i + 1], buf[i + 2]);
      }
      FastLEDHub.show();
    }
    else if (protocol == 3) // DRGBW: [r, g, b, w, ...]
    {
      int index = 0;
      for (size_t i = 2; i + 3 < size && index < numLeds; i += 4, index++)
      {
        uint8_t r = qadd8(buf[i], buf[i + 3]);
        uint8_t g = qadd8(buf[i + 1], buf[i + 3]);
        uint8_t b = qadd8(buf[i + 2], buf[i + 3]);
        leds[index] = CRGB(r, g, b);
      }
      FastLEDHub.show();
    }
    else if (protocol == 4) // DNRGB: [offset_hi, offset_lo, r, g, b, ...]
    {
      if (size >= 4)
      {
        int index = (int)(((uint16_t)buf[2] << 8) | buf[3]);
        for (size_t i = 4; i + 2 < size && index < numLeds; i += 3, index++)
        {
          leds[index] = CRGB(buf[i], buf[i + 1], buf[i + 2]);
        }
        FastLEDHub.show();
      }
    }
  }

  static void parseDDPPacket(const uint8_t *buf, size_t size)
  {
    if (size < 10)
      return;

    uint8_t flags = buf[0];
    bool hasTime = (flags & 0x10) != 0;
    size_t headerLen = hasTime ? 14 : 10;
    if (size <= headerLen)
      return;

    uint32_t byteOffset = ((uint32_t)buf[4] << 24) |
                          ((uint32_t)buf[5] << 16) |
                          ((uint32_t)buf[6] << 8)  |
                          ((uint32_t)buf[7]);

    uint16_t dataLen = ((uint16_t)buf[8] << 8) | buf[9];
    if (headerLen + dataLen > size)
    {
      dataLen = size - headerLen;
    }

    activate(2500);

    CRGB *leds = FastLEDHub.leds();
    int numLeds = FastLEDHub.size();
    if (!leds || numLeds <= 0)
      return;

    int startPixel = (int)(byteOffset / 3);
    int index = startPixel;

    for (size_t i = headerLen; i + 2 < headerLen + dataLen && index < numLeds; i += 3, index++)
    {
      leds[index] = CRGB(buf[i], buf[i + 1], buf[i + 2]);
    }

    if ((flags & 0x01) || index >= numLeds || startPixel == 0)
    {
      FastLEDHub.show();
    }
  }

  static void parseHyperionPacket(const uint8_t *buf, size_t size)
  {
    if (size < 3)
      return;

    activate(2500);

    CRGB *leds = FastLEDHub.leds();
    int numLeds = FastLEDHub.size();
    if (!leds || numLeds <= 0)
      return;

    int index = 0;
    for (size_t i = 0; i + 2 < size && index < numLeds; i += 3, index++)
    {
      leds[index] = CRGB(buf[i], buf[i + 1], buf[i + 2]);
    }
    FastLEDHub.show();
  }

  void initialize()
  {
    m_udpInitialized = false;
    m_active = false;
  }

  void handle()
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      m_udpInitialized = false;
      return;
    }

    if (!m_udpInitialized)
    {
      m_udpRealtime.begin(REALTIME_UDP_PORT);
      m_udpDDP.begin(REALTIME_DDP_PORT);
      m_udpHyperion.begin(REALTIME_HYPERION_PORT);
      m_udpInitialized = true;
      PRINTLN_VERBOSE("[RealtimeStream] UDP listeners started (WLED: 21324, DDP: 4048, Hyperion: 19446)");
    }

    // 1. WLED Realtime UDP
    int packetSize = m_udpRealtime.parsePacket();
    while (packetSize > 0)
    {
      if (packetSize <= (int)sizeof(m_packetBuffer))
      {
        int len = m_udpRealtime.read(m_packetBuffer, sizeof(m_packetBuffer));
        if (len > 0)
        {
          parseWLEDPacket(m_packetBuffer, len);
        }
      }
      else
      {
        m_udpRealtime.flush();
      }
      packetSize = m_udpRealtime.parsePacket();
    }

    // 2. DDP
    packetSize = m_udpDDP.parsePacket();
    while (packetSize > 0)
    {
      if (packetSize <= (int)sizeof(m_packetBuffer))
      {
        int len = m_udpDDP.read(m_packetBuffer, sizeof(m_packetBuffer));
        if (len > 0)
        {
          parseDDPPacket(m_packetBuffer, len);
        }
      }
      else
      {
        m_udpDDP.flush();
      }
      packetSize = m_udpDDP.parsePacket();
    }

    // 3. Hyperion Raw UDP
    packetSize = m_udpHyperion.parsePacket();
    while (packetSize > 0)
    {
      if (packetSize <= (int)sizeof(m_packetBuffer))
      {
        int len = m_udpHyperion.read(m_packetBuffer, sizeof(m_packetBuffer));
        if (len > 0)
        {
          parseHyperionPacket(m_packetBuffer, len);
        }
      }
      else
      {
        m_udpHyperion.flush();
      }
      packetSize = m_udpHyperion.parsePacket();
    }

    // 4. Timeout check
    if (m_active)
    {
      if (m_timeoutMs != UINT32_MAX && millis() > m_timeout)
      {
        stop();
      }
    }
  }

  bool isActive()
  {
    return m_active;
  }

  void stop()
  {
    if (!m_active)
      return;

    m_active = false;
    PRINTLN_VERBOSE("[RealtimeStream] Live streaming stopped (timeout / ended)");

    if (FastLEDHub.getStatus() == STOPPED)
    {
      FastLEDHub.clear(true);
    }
    else
    {
      FastLEDHub.show();
    }
  }
}
