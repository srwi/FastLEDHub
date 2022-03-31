#pragma once

#include "Config.h"

#include <Arduino.h>

namespace Fade
{

  enum class FadeMode
  {
    NONE,
    ALARM,
    SUNSET
  };

  void handle();
  void begin(FadeMode fadeMode);
  void stop();
  void initialize();
  FadeMode getMode();
  uint16_t getBrightness10();

} // namespace Fade
