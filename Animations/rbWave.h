#pragma once

#include "Animation.h"
#include "Config.h"
#include "FastLEDManager.h"

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
    for (uint16_t i = 0; i < FastLEDManager.numLeds; i++)
    {
      FastLEDManager.leds[i] = getCyclicColor((ledDiv * i + step) % (HSV2RGB_SMOOTH_RANGE * 2 / 3));
    }

    step++;
    if (step == HSV2RGB_SMOOTH_RANGE * 2 / 3)
      step = 0;

    FastLEDManager.show();
  }

private:
  // Division will leave a small remainder but won't be noticable
  uint8_t ledDiv = 2.0 / 3 * HSV2RGB_SMOOTH_RANGE / FastLEDManager.numLeds;
  uint16_t step;

  CRGB getCyclicColor(uint16_t index)
  {
    if (index < HSV2RGB_SMOOTH_RANGE * 1 / 3)
    {
      return hsv2rgb_smooth(HSV2RGB_SMOOTH_RANGE * 2 / 3 + index, Config.saturation, 255);
    }
    else
    {
      return hsv2rgb_smooth(HSV2RGB_SMOOTH_RANGE - (index - HSV2RGB_SMOOTH_RANGE * 1 / 3), Config.saturation, 255);
    }
  }
};
