#include <ESPEssentials.h>
#include <FastLEDHub.h>

#include "Animations/rbWave.h"
#include "Animations/rgbWave.h"

#define NUM_LEDS 100
#define LIGHTSTRIP_PIN 5

void setup()
{
  FastLEDHub.initialize("FastLEDHubExample", NUM_LEDS);
  FastLEDHub.addLeds<WS2812B, LIGHTSTRIP_PIN, GRB>(FastLEDHub.hardwareLeds, NUM_LEDS);
  FastLEDHub.registerAnimation(new RbWave("RB Wave"));
  FastLEDHub.registerAnimation(new RgbWave("RGB Wave"));
  FastLEDHub.registerSlider(new Slider("Saturation", 150, 255, 170, 1));
}

void loop()
{
  FastLEDHub.handle();
}
