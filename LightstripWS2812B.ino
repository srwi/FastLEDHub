#include <Arduino.h>
#include <ESPEssentials.h>

#include "Animation.h"
#include "WebSocket.h"
#include "Hardware.h"
#include "Config.h"
#include "WebUpdate.h"

void setup()
{
  initESPEssentials("Lightstrip");
  Config.init();
  initHardware();
  initWebsocket();
  initWebUpdate();
  registerAnimations();
}

void loop()
{
  handleESPEssentials();
  webSocket.loop();
  Animation::current->loop();
  betterShow();
}