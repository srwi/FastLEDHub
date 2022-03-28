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
    CRGB color = strtol(Config.color.c_str(), NULL, 16);
    FastLEDHub.showColor(color);
  }
};
