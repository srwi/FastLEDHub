#pragma once

#include <Arduino.h>
#include <Ticker.h>
// Suppress FastLED warnings
#define FASTLED_INTERNAL
#include "FastLED.h"

#include "Animation.h"
#include "Fade.h"

#define NUM_LEDS 6
#define LIGHTSTRIP_PIN 5
#define BUTTON_PIN 4

#define MAX_BETTER_HUE 1535

extern CRGB leds[NUM_LEDS];
extern CRGB brightnessCorrectedLeds[NUM_LEDS];
extern int16_t brightness10;
extern float filteredBrightness;

void initHardware();
void showshow();
void betterShow(int16_t newBrightness = -1);
void betterClear();
CRGB betterHue(uint16_t fract1535, int16_t sat = -1, uint8_t val = 255);
uint16_t getPotiBrightness();
void handleInput();

struct FastLEDDummy
{
  static void show() { betterShow(); };
};
#define FastLED \
  FastLEDDummy {}