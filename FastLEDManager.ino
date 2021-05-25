#include "Animation.h"
#include "Config.h"
#include "Fade.h"
#include "FastLEDManager.h"
#include "Webserver.h"
#include "WebSocket.h"

#include "Animations/color.h"
#include "Animations/rbWave.h"
#include "Animations/rgbWave.h"

#include <Arduino.h>
#include <ESPEssentials.h>
#include <FastLED.h>

#define LIGHTSTRIP_PIN 5
#define BUTTON_PIN 4


void setup()
{
  initESPEssentials("Lightstrip");

  FastLEDManager.initialize();
  FastLEDManager.enableCycleButton(4);
  FastLEDManager.enablePotentiometer(5);

  FastLEDManager.addLeds<WS2812B, LIGHTSTRIP_PIN, GRB>(FastLEDManager.brightnessCorrectedLeds, NUM_LEDS);

  // FastLEDManager.setDither(0);
  // FastLEDManager.setTemperature(Tungsten100W);
  // FastLEDManager.setBrightness(255);

  FastLEDManager.registerAnimation(new Color("Color"));
  FastLEDManager.registerAnimation(new RbWave("RB Wave"));
  FastLEDManager.registerAnimation(new RgbWave("RGB Wave"));
}

void loop()
{
  handleESPEssentials();
  FastLEDManager.handle();
}
