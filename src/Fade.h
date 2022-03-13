#pragma once

#include "Config.h"

#include <Arduino.h>


class FastLEDManagerClass;

namespace Fade
{

enum class FadeMode
{
  NONE = 0,
  ALARM,
  SUNSET
};

extern FadeMode currentFade;

void handle();
void begin(FadeMode fadeMode);
void stop();
void tick();
void initialize();
void getSunsetTime();

} // namespace Fade
