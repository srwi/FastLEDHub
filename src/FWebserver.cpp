#include "FWebserver.h"

#include "Animation.h"
#include "Fade.h"
#include "FastLEDHub.h"

#include <Arduino.h>
#include <ESPEssentials.h>

namespace Webserver
{

  void initialize()
  {
    ESPEssentials::WebServer.on("/reboot", HTTP_GET, [&]()
                                { ESP.restart(); });
    ESPEssentials::WebServer.on("/sunset", HTTP_GET, [&]()
                                {
                                  Fade::begin(Fade::FadeMode::SUNSET);
                                  ESPEssentials::WebServer.send(200, "text/plain", "Starting sunset.");
                                });
    ESPEssentials::WebServer.on("/alarm", HTTP_GET, [&]()
                                {
                                  Fade::begin(Fade::FadeMode::ALARM);
                                  ESPEssentials::WebServer.send(200, "text/plain", "Starting alarm.");
                                });
    ESPEssentials::WebServer.on("/stop", HTTP_GET, [&]()
                                {
                                  Fade::stop();
                                  FastLEDHub.stop();
                                  ESPEssentials::WebServer.send(200, "text/plain", "Animation stopped.");
                                });
    ESPEssentials::WebServer.on("/pause", HTTP_GET, [&]()
                                {
                                  FastLEDHub.pause();
                                  ESPEssentials::WebServer.send(200, "text/plain", "Animation paused.");
                                });
    ESPEssentials::WebServer.on("/resume", HTTP_GET, [&]()
                                {
                                  FastLEDHub.resume();
                                  ESPEssentials::WebServer.send(200, "text/plain", "Animation resumed.");
                                });
    ESPEssentials::WebServer.on("/toggle", HTTP_GET, [&]()
                                {
                                  FastLEDHub.toggle();
                                  ESPEssentials::WebServer.send(200, "text/plain", "Animation toggled.");
                                });
    ESPEssentials::WebServer.on("/restart", HTTP_GET, [&]()
                                {
                                  FastLEDHub.restart();
                                  ESPEssentials::WebServer.send(200, "text/plain", "Animation restarted.");
                                });
    ESPEssentials::WebServer.on("/status", HTTP_GET, [&]()
                                {
                                  String statusMsg = "{\"status\": " + String((int) FastLEDHub.getStatus()) + ", \"currentAnimation\": \"" + FastLEDHub.getCurrentAnimationName() + "\"}";
                                  ESPEssentials::WebServer.send(200, "text/plain", statusMsg.c_str());
                                });
    ESPEssentials::WebServer.on("/begin", HTTP_GET, [&]()
                                {
                                  if (ESPEssentials::WebServer.hasArg("animation"))
                                  {
                                    String animationName = String(ESPEssentials::WebServer.arg("animation"));
                                    FastLEDHub.begin(FastLEDHub.getAnimation(animationName));
                                    ESPEssentials::WebServer.send(200, "text/plain", "Started '" + animationName + "'");
                                  }
                                  else if (ESPEssentials::WebServer.hasArg("index"))
                                  {
                                    uint8_t animationIndex = ESPEssentials::WebServer.arg("index").toInt();
                                    FastLEDHub.begin(FastLEDHub.getAnimation(animationIndex));
                                    ESPEssentials::WebServer.send(200, "text/plain", "Started animation #" + String(animationIndex));
                                  }
                                  else if (Config.startupAnimation != "")
                                  {
                                    FastLEDHub.begin(FastLEDHub.getAnimation(Config.startupAnimation));
                                    ESPEssentials::WebServer.send(200, "text/plain", "Started '" + Config.startupAnimation + "'");
                                  }
                                  else
                                  {
                                    ESPEssentials::WebServer.send(200, "text/plain", "No startup animation defined.");
                                  }
                                });
  }

} // namespace Webserver
