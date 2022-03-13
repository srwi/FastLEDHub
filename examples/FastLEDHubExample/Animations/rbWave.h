#pragma once

#include <FastLEDHub.h>

class RbWave : public Animation
{
public:
  using Animation::Animation;

  void reset()
  {
    step = 0;
  }

  void loop()
  {
    for (uint16_t i = 0; i < FastLEDHub.numLeds; i++)
    {
      FastLEDHub.leds[i] = getCyclicColor((ledDiv * i + step) % (HSV2RGB_SMOOTH_RANGE * 2 / 3));
    }

    step++;
    if (step == HSV2RGB_SMOOTH_RANGE * 2 / 3)
      step = 0;

    FastLEDHub.delay(5);
    FastLEDHub.show();
  }

private:
  uint8_t ledDiv = 2.0 / 3 * HSV2RGB_SMOOTH_RANGE / FastLEDHub.numLeds;
  uint16_t step;

  CRGB getCyclicColor(uint16_t index)
  {
    if (index < HSV2RGB_SMOOTH_RANGE * 1 / 3)
    {
      return hsv2rgb_smooth(HSV2RGB_SMOOTH_RANGE * 2 / 3 + index, FastLEDHub.getSlider("Saturation")->value, 255);
    }
    else
    {
      return hsv2rgb_smooth(HSV2RGB_SMOOTH_RANGE - (index - HSV2RGB_SMOOTH_RANGE * 1 / 3), FastLEDHub.getSlider("Saturation")->value, 255);
    }
  }
};
