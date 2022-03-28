#pragma once

#include <FastLEDHub.h>

class RgbWave : public Animation
{
public:
  using Animation::Animation;

  uint8_t ledDiv = HSV2RGB_SMOOTH_RANGE / FastLEDHub.size();
  uint16_t step;

  void reset()
  {
    step = 0;
  }

  void loop()
  {
    for (uint16_t i = 0; i < FastLEDHub.size(); i++)
    {
      FastLEDHub.leds()[i] = hsv2rgb_smooth((ledDiv * i + step) % HSV2RGB_SMOOTH_RANGE, FastLEDHub.getSlider("Saturation")->value, 255);
    }

    step++;
    if (step == HSV2RGB_SMOOTH_RANGE)
      step = 0;

    FastLEDHub.delay(5);
    FastLEDHub.show();
  }
};
