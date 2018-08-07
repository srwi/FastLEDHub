#ifndef _CUSTOMCOLOR_
#define _CUSTOMCOLOR_

#include <Arduino.h>

#include "../EffectController.h"

extern Effect customColor;

namespace customColorNamespace
{
	extern CRGB currentCustomColor;
	extern CRGB newCustomColor;

	extern void set(CRGB newColor);

	extern void reset();
	extern void tick();
}

#endif