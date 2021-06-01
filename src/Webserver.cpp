#include "Webserver.h"

#include "Animation.h"
#include "Fade.h"
#include "FastLEDManager.h"

#include <Arduino.h>
#include <ESPEssentials.h>


namespace Webserver
{

void initialize()
{
  WebServer.on("/reboot", HTTP_GET, [&]() {
    ESP.restart();
  });
  WebServer.on("/sunset", HTTP_GET, [&]() {
    Fade::begin(Fade::FadeMode::SUNSET);
    WebServer.send(200, "text/plain", "Starting sunset.");
  });
  WebServer.on("/alarm", HTTP_GET, [&]() {
    Fade::begin(Fade::FadeMode::ALARM);
    WebServer.send(200, "text/plain", "Starting alarm.");
  });
  WebServer.on("/stop", HTTP_GET, [&]() {
    Fade::stop();
    FastLEDManager.stop();
    WebServer.send(200, "text/plain", "Animation stopped.");
  });
  WebServer.on("/pause", HTTP_GET, [&]() {
    FastLEDManager.pause();
    WebServer.send(200, "text/plain", "Animation paused.");
  });
  WebServer.on("/resume", HTTP_GET, [&]() {
    FastLEDManager.resume();
    WebServer.send(200, "text/plain", "Animation resumed.");
  });
  WebServer.on("/toggle", HTTP_GET, [&]() {
    FastLEDManager.toggle();
    WebServer.send(200, "text/plain", "Animation toggled.");
  });
  WebServer.on("/restart", HTTP_GET, [&]() {
    FastLEDManager.restart();
    WebServer.send(200, "text/plain", "Animation restarted.");
  });
  WebServer.on("/begin", HTTP_GET, [&]() {
    if (WebServer.hasArg("animation"))
    {
      String animationName = String(WebServer.arg("animation"));
      FastLEDManager.begin(FastLEDManager.getAnimation(animationName));
      WebServer.send(200, "text/plain", "Started '" + animationName + "'");
    }
    else if (WebServer.hasArg("index"))
    {
      uint8_t animationIndex = WebServer.arg("index").toInt();
      if(animationIndex < FastLEDManager.animations.size())
        FastLEDManager.begin(FastLEDManager.getAnimation(animationIndex));
      WebServer.send(200, "text/plain", "Started animation #" + String(animationIndex));
    }
    else
    {
      FastLEDManager.begin(FastLEDManager.getAnimation(0));
    }
  });
}

} // namespace Webserver
