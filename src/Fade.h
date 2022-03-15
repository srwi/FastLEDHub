#pragma once

#include "Config.h"

#include <Arduino.h>


class FastLEDHubClass;

namespace Fade
{

enum class FadeMode
{
  NONE,
  ALARM,
  SUNSET
};

extern FadeMode mode;

void tick();
void handle();
void begin(FadeMode fadeMode);
void stop();
void initialize();
void getSunsetTime();
bool getCurrentTime(int8_t *hour, int8_t *minute);

} // namespace Fade
