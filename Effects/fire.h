#ifndef _FIRE_H
#define _FIRE_H

#include <Arduino.h>

#include "../EffectController.h"

extern Effect fire;

namespace fireNamespace
{
	extern void reset();
	extern void tick();
}

#endif