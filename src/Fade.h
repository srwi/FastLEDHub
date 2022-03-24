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

} // namespace Fade
