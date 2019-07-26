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
  if(!Animation::isDelaying())
    Animation::getCurrent()->loop();

  handleESPEssentials();
  webSocket.loop();
  betterShow();
}