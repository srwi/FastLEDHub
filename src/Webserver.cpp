#include "Webserver.h"

#include "Animation.h"
#include "Fade.h"
#include "FastLEDHub.h"

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
    FastLEDHub.stop();
    WebServer.send(200, "text/plain", "Animation stopped.");
  });
  WebServer.on("/pause", HTTP_GET, [&]() {
    FastLEDHub.pause();
    WebServer.send(200, "text/plain", "Animation paused.");
  });
  WebServer.on("/resume", HTTP_GET, [&]() {
    FastLEDHub.resume();
    WebServer.send(200, "text/plain", "Animation resumed.");
  });
  WebServer.on("/toggle", HTTP_GET, [&]() {
    FastLEDHub.toggle();
    WebServer.send(200, "text/plain", "Animation toggled.");
  });
  WebServer.on("/restart", HTTP_GET, [&]() {
    FastLEDHub.restart();
    WebServer.send(200, "text/plain", "Animation restarted.");
  });
  WebServer.on("/begin", HTTP_GET, [&]() {
    if (WebServer.hasArg("animation"))
    {
      String animationName = String(WebServer.arg("animation"));
      FastLEDHub.begin(FastLEDHub.getAnimation(animationName));
      WebServer.send(200, "text/plain", "Started '" + animationName + "'");
    }
    else if (WebServer.hasArg("index"))
    {
      uint8_t animationIndex = WebServer.arg("index").toInt();
      if(animationIndex < FastLEDHub.animations.size())
        FastLEDHub.begin(FastLEDHub.getAnimation(animationIndex));
      WebServer.send(200, "text/plain", "Started animation #" + String(animationIndex));
    }
    else
    {
      FastLEDHub.begin(FastLEDHub.getAnimation(0));
    }
  });
}

} // namespace Webserver
