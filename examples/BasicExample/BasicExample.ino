#include <FastLEDHub.h>

#include "Animations/Confetti.h"
#include "Animations/Color.h"
#include "Animations/Gradient.h"
#include "Animations/Rainbow.h"
#include "Animations/RGBWave.h"

#define NUM_LEDS 100
#define LIGHTSTRIP_PIN 5
#define LED_TYPE WS2812B

CRGB leds[NUM_LEDS];

void setup()
{
  FastLEDHub.initialize("Basic Example");
  FastLEDHub.addLeds<LED_TYPE, LIGHTSTRIP_PIN, GRB>(leds, NUM_LEDS);

  FastLEDHub.registerAnimation(new Color("Color"));
  FastLEDHub.registerAnimation(new Gradient("Gradient"));
  FastLEDHub.registerAnimation(new Rainbow("Rainbow"));
  FastLEDHub.registerAnimation(new RGBWave("RGB Wave"));
  FastLEDHub.registerAnimation(new Confetti("Confetti"));

  FastLEDHub.registerSlider(new Slider("Saturation", 0, 255, 255, 1, "palette2"));

  FastLEDHub.registerColorPicker(new ColorPicker("Primary Color", CRGB(255, 0, 0)));
  FastLEDHub.registerColorPicker(new ColorPicker("Secondary Color", CRGB(0, 255, 0)));
}

void loop()
{
  FastLEDHub.handle();
}
