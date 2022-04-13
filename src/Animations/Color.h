#pragma once

#include "ColorUtils.h"

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
    CRGB color = FastLEDHub.getColorPicker(0)->value;
    FastLEDHub.showColor(color);
  }
};
