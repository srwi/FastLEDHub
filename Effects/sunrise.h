#ifndef _SUNRISE_H
#define _SUNRISE_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern EffectConfiguration sunrise;

namespace sunriseNamespace
{
	extern void reset();
	extern void tick();
}

#endif