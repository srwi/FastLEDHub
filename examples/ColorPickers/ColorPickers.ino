#include <FastLEDHub.h>

#include <Animations/Color.h>  // This animation comes with FastLEDHub
#include "Animations/Gradient.h"

#define NUM_LEDS 100
#define LIGHTSTRIP_PIN 5
#define LED_TYPE WS2812B

CRGB leds[NUM_LEDS];

void setup()
{
  FastLEDHub.initialize("Color Pickers Example");
  FastLEDHub.addLeds<LED_TYPE, LIGHTSTRIP_PIN, GRB>(leds, NUM_LEDS);

  FastLEDHub.registerAnimation(new Color("Color"));
  FastLEDHub.registerAnimation(new Gradient("Gradient"));

  FastLEDHub.registerColorPicker(new ColorPicker("Primary Color", CRGB(255, 0, 0)));
  FastLEDHub.registerColorPicker(new ColorPicker("Secondary Color", CRGB(255, 0, 0)));
}

void loop()
{
  FastLEDHub.handle();
}
