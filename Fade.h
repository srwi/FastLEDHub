#ifndef _FADE_H
#define _FADE_H

#include <Arduino.h>
#include <Ticker.h>

#include "Hardware.h"
#include "Time.h"
#include "Config.h"
#include "Animation.h"

enum FadeMode
{
  NONE = 0,
  ALARM,
  SUNSET
};

extern FadeMode currentFade;

void handleFade();
void startFade(FadeMode fadeMode);
void stopFade();
void fadeTick();

#endif