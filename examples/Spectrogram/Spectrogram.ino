#include <FastLEDHub.h>

#include "Animations/Spectrogram.h"

#define NUM_LEDS 100
#define LIGHTSTRIP_PIN 5
#define LED_TYPE WS2812B

CRGB leds[NUM_LEDS];

void setup()
{
  FastLEDHub.initialize("Spectrogram Example");
  FastLEDHub.addLeds<LED_TYPE, LIGHTSTRIP_PIN, GRB>(leds, NUM_LEDS);

  FastLEDHub.registerAnimation(new Spectrogram("Spectrogram"));
}

void loop()
{
  FastLEDHub.handle();
}
