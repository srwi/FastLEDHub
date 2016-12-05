#ifndef _STATICSINGLECOLOR_H
#define _STATICSINGLECOLOR_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern Effect staticSingleColor;

namespace staticSingleColorNamespace
{
	extern rgb currentStaticSingleColor;

	extern void reset();
	extern void tick();
}

#endif