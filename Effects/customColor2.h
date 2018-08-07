#ifndef _CUSTOMCOLOR2_
#define _CUSTOMCOLOR2_

#include <Arduino.h>

#include "../EffectController.h"

extern Effect customColor2;

namespace customColor2Namespace
{
	extern CRGB currentCustomColor;
	extern CRGB newCustomColor;

	extern void set(CRGB newColor);

	extern void reset();
	extern void tick();
}

#endif