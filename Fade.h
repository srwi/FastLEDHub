#ifndef _FADE_H
#define _FADE_H

#include <Ticker.h>
#include "FastLED.h"

#include "Time.h"
#include "Config.h"

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