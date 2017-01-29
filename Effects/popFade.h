#ifndef _POPFADE_H
#define _POPFADE_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern Effect popFade;

namespace popFadeNamespace
{
	extern void reset();
	extern void tick();
}

#endif