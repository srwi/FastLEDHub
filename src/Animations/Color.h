#pragma once

#include "ColorUtils.h"

#include <FastLEDHub.h>

class Color : public Animation
{
public:
  using Animation::Animation;

  CRGB _color;

  void setColor(CRGB color) {
    _color = color;
    FastLEDHub.getColorPicker(0)->value = _color;
  }
  void reset() {
  }

  void loop() {
    _color = FastLEDHub.getColorPicker(0)->value;
    FastLEDHub.showColor(_color);
  }
};
