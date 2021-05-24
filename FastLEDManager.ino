#include "Animation.h"
#include "Config.h"
#include "Fade.h"
#include "FastLEDManager.h"
#include "Webserver.h"
#include "WebSocket.h"

// Include animations here:
#include "Animations/color.h"

#include <Arduino.h>
#include <ESPEssentials.h>
#include <FastLED.h>

#define LIGHTSTRIP_PIN 5
#define BUTTON_PIN 4


void setup()
{
  initESPEssentials("Lightstrip");

  FastLEDManager.initialize();
  FastLEDManager.setButtonPin(4);
  FastLEDManager.setPotiPin(5);

  FastLEDManager.addLeds<WS2812B, LIGHTSTRIP_PIN, GRB>(FastLEDManager.brightnessCorrectedLeds, NUM_LEDS);

  // FastLEDManager.setDither(0);
  // FastLEDManager.setTemperature(Tungsten100W);
  // FastLEDManager.setBrightness(255);

  FastLEDManager.registerAnimation(new Color("Color"));
}

void loop()
{
  handleESPEssentials();
  FastLEDManager.handle();
}
