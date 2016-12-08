#ifndef _CUSTOMCOLOR_
#define _CUSTOMCOLOR_

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern EffectConfiguration customColor;

namespace customColorNamespace
{
	extern CRGB currentCustomColor;
	extern CRGB newCustomColor;

	extern void set(CRGB newColor);

	extern void reset();
	extern void tick();
}

#endif