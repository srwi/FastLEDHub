#include "Hardware.h"

Ticker inputTicker;
bool buttonPushed = false;
float filteredBrightness = 128;
int16_t brightness10 = 1023;
CRGB leds[NUM_LEDS];
CRGB brightnessCorrectedLeds[NUM_LEDS];

void betterShow(int16_t bright10)
{
  if (bright10 != -1)
    brightness10 = bright10;

  int16_t gammaCorrectedBrightness = round((float)brightness10 * brightness10 / 1023);
  uint8_t bright8 = gammaCorrectedBrightness / 4;
  uint8_t fract2 = bright8 != 255 ? gammaCorrectedBrightness % 4 : 0;

  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    brightnessCorrectedLeds[i] = leds[i];
    switch (fract2)
    {
    case 0:
      brightnessCorrectedLeds[i].nscale8(bright8);
      break;
    case 2:
      if (i % 2)
      {
        brightnessCorrectedLeds[i].nscale8(bright8);
      }
      else
      {
        brightnessCorrectedLeds[i].nscale8(bright8 + 1);
      }
      break;
    case 1:
    case 3:
      if (i % 4 < fract2)
      {
        brightnessCorrectedLeds[i].nscale8(bright8 + 1);
      }
      else
      {
        brightnessCorrectedLeds[i].nscale8(bright8);
      }
      break;
    }
  }

#pragma push_macro("FastLED")
#undef FastLED
  FastLED.show();
#pragma pop_macro("FastLED")
}

void betterClear()
{
  for(uint8_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
  betterShow();
}

CRGB betterHue(uint16_t fract1535, int16_t sat, uint8_t val)
{
  // fract1535 is a number from 0 to 1535 (MAX_BETTER_HUE)
  uint8_t fractHue = fract1535 / 6;
  uint8_t fractBlend = fract1535 % 6;
  if (sat == -1)
    sat = 255;

  CRGB leftColor, rightColor;
  hsv2rgb_rainbow(CHSV(fractHue, sat, val), leftColor);
  hsv2rgb_rainbow(CHSV(fractHue + 1, sat, val), rightColor);

  return blend(leftColor, rightColor, 42 * fractBlend);
}

void initHardware()
{
  pinMode(BUTTON_PIN, INPUT);
  inputTicker.attach_ms(10, handleInput);
#pragma push_macro("FastLED")
#undef FastLED
  FastLED.addLeds<WS2812B, LIGHTSTRIP_PIN, GRB>(brightnessCorrectedLeds, NUM_LEDS);
  FastLED.setDither(0);
  FastLED.setTemperature(Tungsten100W);
  FastLED.setBrightness(255);
#pragma pop_macro("FastLED")
}

uint16_t getPotiBrightness()
{
  int16_t value = (1023 - analogRead(A0) - 29) * 1.06;

  if (value > 1023)
    value = 1023;
  if (value < 0)
    value = 0;

  return value;
}

void handleInput()
{
  if (!currentFade)
  {
    // Adjust brightness calculation if needed
    uint16_t potiBrightness = getPotiBrightness();

    filteredBrightness = filteredBrightness - 0.01 * (filteredBrightness - potiBrightness);

    // Only set brightness if it's not near the filtered brightness value which will lag behind
    if (!(filteredBrightness - 1 < potiBrightness && potiBrightness < filteredBrightness + 1))
    {
      brightness10 = (float)potiBrightness * potiBrightness / 1023;
    }
  }

  // Push button
  if (!digitalRead(BUTTON_PIN) && !buttonPushed)
  {
    // button pushed
    stopFade();
    beginNextAnimation();

    buttonPushed = true;
  }
  else if (digitalRead(BUTTON_PIN) && buttonPushed)
  {
    // button released

    buttonPushed = false;
  }
}