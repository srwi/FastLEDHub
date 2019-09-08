#include "Webserver.h"

void initWebserverCommands()
{
  WebServer.on("/reboot", HTTP_GET, [&]() {
    ESP.restart();
  });
  WebServer.on("/sunset", HTTP_GET, [&]() {
    startFade(SUNSET);
    WebServer.send(200, "text/plain", "Starting sunset.");
  });
  WebServer.on("/alarm", HTTP_GET, [&]() {
    startFade(ALARM);
    WebServer.send(200, "text/plain", "Starting alarm.");
  });
  WebServer.on("/stop", HTTP_GET, [&]() {
    stopFade();
    if (currentAnimation)
      currentAnimation->stop();
    WebServer.send(200, "text/plain", "Animation stopped.");
  });
  WebServer.on("/pause", HTTP_GET, [&]() {
    if (currentAnimation)
      currentAnimation->pause();
    WebServer.send(200, "text/plain", "Animation paused.");
  });
  WebServer.on("/resume", HTTP_GET, [&]() {
    if (currentAnimation)
      currentAnimation->resume();
    WebServer.send(200, "text/plain", "Animation resumed.");
  });
  WebServer.on("/toggle", HTTP_GET, [&]() {
    if (currentAnimation)
      currentAnimation->toggle();
    WebServer.send(200, "text/plain", "Animation toggled.");
  });
  WebServer.on("/restart", HTTP_GET, [&]() {
    if (currentAnimation)
      currentAnimation->restart();
    WebServer.send(200, "text/plain", "Animation restarted.");
  });
  WebServer.on("/begin", HTTP_GET, [&]() {
    if (WebServer.hasArg("animation"))
    {
      String animationName = String(WebServer.arg("animation"));
      getAnimation(animationName)->begin();
      WebServer.send(200, "text/plain", "Started '" + animationName + "'");
    }
    else if (WebServer.hasArg("index"))
    {
      uint8_t animationIndex = WebServer.arg("index").toInt();
      if(animationIndex < animations.size())
        getAnimation(animationIndex)->begin();
      WebServer.send(200, "text/plain", "Started animation #" + String(animationIndex));
    }
    else
    {
      getAnimation(0)->begin();
    }
  });
}