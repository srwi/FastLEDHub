#pragma once

#include <FastLEDHub.h>

class Color : public Animation
{
public:
  using Animation::Animation;

  void reset()
  {
  }

  void loop()
  {
    fill_solid(FastLEDHub.leds, FastLEDHub.numLeds, strtol((const char *)&Config.color[0], NULL, 16));

    FastLEDHub.show();
  }
};
