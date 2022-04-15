#include <FastLEDHub.h>

#include "Animations/Rainbow.h"

#define NUM_LEDS 100
#define LIGHTSTRIP_PIN 5
#define LED_TYPE WS2812B

CRGB leds[NUM_LEDS];

void setup()
{
  FastLEDHub.initialize("Sliders Example");
  FastLEDHub.addLeds<LED_TYPE, LIGHTSTRIP_PIN, GRB>(leds, NUM_LEDS);

  FastLEDHub.registerAnimation(new Rainbow("Rainbow"));

  FastLEDHub.registerSlider(new Slider("Saturation", 0, 255, 255, 1, "palette2"));
}

void loop()
{
  FastLEDHub.handle();
}
