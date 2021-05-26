#pragma once

#include "Animation.h"
#include "Config.h"
#include "FastLEDManager.h"

class RgbWave : public Animation
{
public:
  using Animation::Animation;

  // Division will leave a small remainder but won't be noticable
  uint8_t ledDiv = HSV2RGB_SMOOTH_RANGE / FastLEDManager.numLeds;
  uint16_t step;

  void reset()
  {
    step = 0;
  }

  void loop()
  {
    for (uint16_t i = 0; i < FastLEDManager.numLeds; i++)
    {
      FastLEDManager.leds[i] = hsv2rgb_smooth((ledDiv * i + step) % HSV2RGB_SMOOTH_RANGE, Config.saturation, 255);
    }

    step++;
    if (step == HSV2RGB_SMOOTH_RANGE)
      step = 0;

    delay(5);

    FastLEDManager.show();
  }
};
