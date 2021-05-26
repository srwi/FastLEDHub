#pragma once

#include "Animation.h"
#include "Config.h"
#include "FastLEDManager.h"

class Color : public Animation
{
public:
  using Animation::Animation;

  void reset()
  {
  }

  void loop()
  {
    fill_solid(FastLEDManager.leds, FastLEDManager.numLeds, strtol((const char *)&Config.color[0], NULL, 16));

    FastLEDManager.show();
  }
};
