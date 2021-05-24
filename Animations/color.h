#pragma once

#include "Animation.h"
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
    fill_solid(FastLEDManager.leds, NUM_LEDS, strtol((const char *)&FastLEDManager.config->color[0], NULL, 16));

    FastLEDManager.show();
  }
};
