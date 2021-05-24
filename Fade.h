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
void initialize(ConfigClass *conf);
void getSunset(uint16_t d, float Lat, float Long);
float rad(float deg);

} // namespace Fade
