#include <Arduino.h>
#include <ESPEssentials.h>

#include "Animation.h"
#include "Config.h"
#include "Fade.h"
#include "Hardware.h"
#include "WebSocket.h"
#include "Webserver.h"

// Include animations here:
#include "Animations/rainbow.h"
#include "Animations/color.h"
#include "Animations/colorBlend.h"
#include "Animations/fader.h"
#include "Animations/colorRider.h"
#include "Animations/rbWave.h"
#include "Animations/rgbWave.h"

void setup()
{
  initESPEssentials("Lightstrip");
  Config.init();
  initHardware();

  if (WiFi.status() == WL_CONNECTED)
  {
    initWebsocket();
    initWebserverCommands();
    initFade();
  }

  // Register animations here:
  registerAnimation(new Rainbow("Rainbow"));
  registerAnimation(new ColorBlend("Color Blend"));
  registerAnimation(new Color("Color"));
  registerAnimation(new Fader("Fader"));
  registerAnimation(new ColorRider("Color Rider"));
  registerAnimation(new RbWave("RB Wave"));
  registerAnimation(new RgbWave("RGB Wave"));

  if (Config.startupAnimation != "")
    getAnimation(Config.startupAnimation)->begin();
}

void loop()
{
  if (!isDelaying && status == RUNNING && currentAnimation)
    currentAnimation->loop();

  handleESPEssentials();
  handleFade();
  webSocket.loop();
  betterShow();
}