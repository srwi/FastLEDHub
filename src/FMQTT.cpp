#include "FMQTT.h"

#include <ArduinoJson.h>
#include <PubSubClient.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

#include "Config.h"
#include "ColorUtils.h"
#include "FastLEDHub.h"
#include "SerialOut.h"

namespace FMQTT
{
  namespace
  {
    WiFiClient m_wifiClient;
    PubSubClient m_mqttClient(m_wifiClient);
    String m_projectName = "FastLEDHub";
    unsigned long m_lastReconnectAttempt = 0;
    const unsigned long RECONNECT_INTERVAL = 10000;

    String getBaseTopic()
    {
      if (Config.mqttTopicName.length() > 0)
        return Config.mqttTopicName;
      return m_projectName;
    }

    String getClientId()
    {
      if (Config.mqttClientId.length() > 0)
        return Config.mqttClientId;

#if defined(ESP8266)
      return m_projectName + "-" + String(ESP.getChipId(), HEX);
#elif defined(ESP32)
      return m_projectName + "-" + String((uint32_t)ESP.getEfuseMac(), HEX);
#else
      return m_projectName;
#endif
    }

    void publishHomeAssistantDiscovery()
    {
      String clientId = getClientId();
      String baseTopic = getBaseTopic();
      String discTopic = "homeassistant/light/" + clientId + "/light/config";

      JsonDocument doc;
      doc["name"] = m_projectName;
      doc["unique_id"] = clientId;
      doc["stat_t"] = baseTopic + "/state";
      doc["cmd_t"] = baseTopic + "/set";
      doc["bri_stat_t"] = baseTopic + "/brightness";
      doc["bri_cmd_t"] = baseTopic + "/brightness/set";
      doc["fx_stat_t"] = baseTopic + "/currentAnimation";
      doc["fx_cmd_t"] = baseTopic + "/animation/set";
      doc["avty_t"] = baseTopic + "/status";
      doc["pl_avail"] = "online";
      doc["pl_not_avail"] = "offline";

      JsonArray fxList = doc["fx_list"].to<JsonArray>();
      for (uint8_t i = 0; i < FastLEDHub.animations.size(); i++)
      {
        fxList.add(FastLEDHub.animations.get(i)->getName());
      }

      if (FastLEDHub.colorPickers.size() > 0)
      {
        doc["rgb_stat_t"] = baseTopic + "/color/rgb";
        doc["rgb_cmd_t"] = baseTopic + "/color/set";
      }

      JsonObject dev = doc["device"].to<JsonObject>();
      dev["identifiers"][0] = clientId;
      dev["name"] = m_projectName;
      dev["model"] = "FastLEDHub";
      dev["manufacturer"] = "srwi";

      String output;
      serializeJson(doc, output);
      m_mqttClient.publish(discTopic.c_str(), output.c_str(), true);
    }

    void onMessage(char *topic, byte *payload, unsigned int length)
    {
      String payloadStr = "";
      for (unsigned int i = 0; i < length; i++)
        payloadStr += (char)payload[i];
      payloadStr.trim();

      String topicStr = String(topic);
      String baseTopic = getBaseTopic();

      PRINTLN("[FMQTT] Message [" + topicStr + "]: " + payloadStr);

      if (!topicStr.startsWith(baseTopic))
        return;

      String subTopic = topicStr.substring(baseTopic.length());
      if (subTopic.startsWith("/"))
        subTopic = subTopic.substring(1);

      if (subTopic == "set" || subTopic == "state/set")
      {
        String upper = payloadStr;
        upper.toUpperCase();
        if (upper == "ON" || upper == "1" || upper == "TRUE")
        {
          if (FastLEDHub.getStatus() == STOPPED)
          {
            if (Config.startupAnimation != "")
              FastLEDHub.begin(FastLEDHub.getAnimation(Config.startupAnimation));
            else if (FastLEDHub.animations.size() > 0)
              FastLEDHub.begin(FastLEDHub.getAnimation(0));
          }
          else if (FastLEDHub.getStatus() == PAUSED)
          {
            FastLEDHub.resume();
          }
        }
        else if (upper == "OFF" || upper == "0" || upper == "FALSE")
        {
          FastLEDHub.stop();
        }
        else if (upper == "TOGGLE")
        {
          FastLEDHub.toggle();
        }
      }
      else if (subTopic == "brightness/set")
      {
        int val = payloadStr.toInt();
        FastLEDHub.setBrightness(constrain(val, 0, 255));
        Slider *bSlider = FastLEDHub.getSlider("Brightness");
        if (bSlider)
          bSlider->value = FastLEDHub.getBrightness();
        if (Config.sliderValues.size() > 0)
          Config.sliderValues.set(0, FastLEDHub.getBrightness());
      }
      else if (subTopic == "speed/set")
      {
        int val = payloadStr.toInt();
        FastLEDHub.setSpeed(constrain(val, 0, 255));
        Slider *sSlider = FastLEDHub.getSlider("Speed");
        if (sSlider)
          sSlider->value = constrain(val, 0, 255);
        if (Config.sliderValues.size() > 1)
          Config.sliderValues.set(1, constrain(val, 0, 255));
      }
      else if (subTopic == "animation/set")
      {
        Animation *a = FastLEDHub.getAnimation(payloadStr);
        if (a)
          FastLEDHub.begin(a);
      }
      else if (subTopic == "cycle")
      {
        FastLEDHub.cycle();
      }
      else if (subTopic == "toggle")
      {
        FastLEDHub.toggle();
      }
      else if (subTopic == "color/set")
      {
        if (FastLEDHub.colorPickers.size() > 0)
        {
          CRGB c;
          if (payloadStr.startsWith("#"))
          {
            c = hex2rgb(payloadStr.substring(1));
          }
          else if (payloadStr.indexOf(',') >= 0)
          {
            int c1 = payloadStr.indexOf(',');
            int c2 = payloadStr.indexOf(',', c1 + 1);
            if (c1 > 0 && c2 > c1)
            {
              int r = payloadStr.substring(0, c1).toInt();
              int g = payloadStr.substring(c1 + 1, c2).toInt();
              int b = payloadStr.substring(c2 + 1).toInt();
              c = CRGB(r, g, b);
            }
          }
          else
          {
            c = hex2rgb(payloadStr);
          }
          FastLEDHub.getColorPicker(0)->value = c;
          if (Config.colorPickerValues.size() > 0)
            Config.colorPickerValues.set(0, c);
        }
      }
      else if (subTopic == "api" || subTopic == "json/set")
      {
        JsonDocument doc;
        if (!deserializeJson(doc, payloadStr))
        {
          if (doc["state"].is<const char *>())
          {
            String s = doc["state"].as<String>();
            s.toUpperCase();
            if (s == "OFF")
              FastLEDHub.stop();
            else if (s == "ON" && FastLEDHub.getStatus() == STOPPED)
              FastLEDHub.toggle();
          }
          if (doc["brightness"].is<int>())
            FastLEDHub.setBrightness(doc["brightness"].as<int>());
          if (doc["animation"].is<const char *>())
          {
            Animation *a = FastLEDHub.getAnimation(doc["animation"].as<String>());
            if (a)
              FastLEDHub.begin(a);
          }
          if (doc["color"].is<const char *>() && FastLEDHub.colorPickers.size() > 0)
          {
            String hex = doc["color"].as<String>();
            if (hex.startsWith("#"))
              hex = hex.substring(1);
            CRGB c = hex2rgb(hex);
            FastLEDHub.getColorPicker(0)->value = c;
            if (Config.colorPickerValues.size() > 0)
              Config.colorPickerValues.set(0, c);
          }
        }
      }

      publishStatus();
    }

    bool connect()
    {
      if (!Config.mqttEnabled || Config.mqttHost.length() == 0 || WiFi.status() != WL_CONNECTED)
        return false;

      m_mqttClient.setBufferSize(1024);
      m_mqttClient.setServer(Config.mqttHost.c_str(), Config.mqttPort > 0 ? Config.mqttPort : 1883);
      m_mqttClient.setCallback(onMessage);

      String clientId = getClientId();
      String baseTopic = getBaseTopic();
      String willTopic = baseTopic + "/status";

      PRINTLN("[FMQTT] Connecting to " + Config.mqttHost + ":" + String(Config.mqttPort) + " as " + clientId);

      bool connected = false;
      const char *user = Config.mqttUsername.length() > 0 ? Config.mqttUsername.c_str() : nullptr;
      const char *pass = Config.mqttPassword.length() > 0 ? Config.mqttPassword.c_str() : nullptr;

      connected = m_mqttClient.connect(clientId.c_str(), user, pass, willTopic.c_str(), 0, true, "offline");

      if (connected)
      {
        PRINTLN("[FMQTT] Connected!");
        m_mqttClient.publish(willTopic.c_str(), "online", true);

        // Subscribe to command topics
        m_mqttClient.subscribe((baseTopic + "/set").c_str());
        m_mqttClient.subscribe((baseTopic + "/state/set").c_str());
        m_mqttClient.subscribe((baseTopic + "/brightness/set").c_str());
        m_mqttClient.subscribe((baseTopic + "/speed/set").c_str());
        m_mqttClient.subscribe((baseTopic + "/animation/set").c_str());
        m_mqttClient.subscribe((baseTopic + "/color/set").c_str());
        m_mqttClient.subscribe((baseTopic + "/cycle").c_str());
        m_mqttClient.subscribe((baseTopic + "/toggle").c_str());
        m_mqttClient.subscribe((baseTopic + "/api").c_str());
        m_mqttClient.subscribe((baseTopic + "/json/set").c_str());

        // Publish available animations list (retained)
        String animationsJson = "[";
        for (uint8_t i = 0; i < FastLEDHub.animations.size(); i++)
        {
          if (i > 0)
            animationsJson += ",";
          animationsJson += """ + FastLEDHub.animations.get(i)->getName() + """;
        }
        animationsJson += "]";
        m_mqttClient.publish((baseTopic + "/animations").c_str(), animationsJson.c_str(), true);

        // Publish Home Assistant MQTT Discovery
        publishHomeAssistantDiscovery();

        publishStatus();
      }
      else
      {
        PRINTLN("[FMQTT] Connect failed, state=" + String(m_mqttClient.state()));
      }

      return connected;
    }
  } // namespace

  void initialize(const String &projectName)
  {
    m_projectName = projectName;
  }

  void handle()
  {
    if (!Config.mqttEnabled || Config.mqttHost.length() == 0 || WiFi.status() != WL_CONNECTED)
      return;

    if (!m_mqttClient.connected())
    {
      unsigned long now = millis();
      if (now - m_lastReconnectAttempt > RECONNECT_INTERVAL || m_lastReconnectAttempt == 0)
      {
        m_lastReconnectAttempt = now;
        connect();
      }
    }
    else
    {
      m_mqttClient.loop();
    }
  }

  bool isConnected()
  {
    return m_mqttClient.connected();
  }

  void publishMessage(const String &subtopic, const String &payload, bool retain)
  {
    if (!m_mqttClient.connected())
      return;

    String topic = getBaseTopic() + "/" + subtopic;
    m_mqttClient.publish(topic.c_str(), payload.c_str(), retain);
  }

  void publishStatus()
  {
    if (!m_mqttClient.connected())
      return;

    String baseTopic = getBaseTopic();
    String stateStr = (FastLEDHub.getStatus() == RUNNING) ? "ON" : "OFF";
    m_mqttClient.publish((baseTopic + "/state").c_str(), stateStr.c_str(), true);
    m_mqttClient.publish((baseTopic + "/brightness").c_str(), String(FastLEDHub.getBrightness()).c_str(), true);
    m_mqttClient.publish((baseTopic + "/currentAnimation").c_str(), FastLEDHub.getCurrentAnimationName().c_str(), true);

    Slider *speedSlider = FastLEDHub.getSlider("Speed");
    if (speedSlider)
      m_mqttClient.publish((baseTopic + "/speed").c_str(), String(speedSlider->value).c_str(), true);

    ColorPicker *cp = FastLEDHub.getColorPicker(0);
    if (cp)
    {
      m_mqttClient.publish((baseTopic + "/color").c_str(), ("#" + rgb2hex(cp->value)).c_str(), true);
      String rgbStr = String(cp->value.r) + "," + String(cp->value.g) + "," + String(cp->value.b);
      m_mqttClient.publish((baseTopic + "/color/rgb").c_str(), rgbStr.c_str(), true);
    }
  }
} // namespace FMQTT
