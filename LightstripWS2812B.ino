#include <Arduino.h>
#include <ESPEssentials.h>

#include "Animation.h"
#include "WebSocket.h"
#include "Hardware.h"
#include "Config.h"
#include "WebUpdate.h"

// Include animations here:
#include "Animations/alternatingRainbow.h"
#include "Animations/color.h"
#include "Animations/confetti.h"
#include "Animations/cremeRainbow.h"
#include "Animations/fader.h"
#include "Animations/fire.h"
#include "Animations/juggle.h"
#include "Animations/leftRightLeftRightLeft.h"
#include "Animations/popFade.h"
#include "Animations/rbRainbow.h"
#include "Animations/rgbRainbow.h"

void setup()
{
  initESPEssentials("Lightstrip");
  Config.init();
  initHardware();
  initWebsocket();
  initWebUpdate();

  // Register animations here:
  registerAnimation(new AlternatingRainbow("Alternating Rainbow"));
  registerAnimation(new Confetti("Confetti"));
  registerAnimation(new CremeRainbow("Creme Rainbow"));
  registerAnimation(new Color("Color"));
  registerAnimation(new ColorFader("Color Fader"));
  registerAnimation(new Fire("Fire"));
  registerAnimation(new Juggle("Juggle"));
  registerAnimation(new LeftRightLeftRightLeft("Left Right Left Right Left"));
  registerAnimation(new PopFade("Pop Fade"));
  registerAnimation(new RbRainbow("RG Rainbow"));
  registerAnimation(new RgbRainbow("RGB Rainbow"));

  getAnimation("Alternating Rainbow")->begin();
}

void loop()
{
  if(!Animation::isDelaying())
    Animation::getCurrent()->loop();

  handleESPEssentials();
  webSocket.loop();
  betterShow();
}