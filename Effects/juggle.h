#ifndef _JUGGLE_H
#define _JUGGLE_H

#include <Arduino.h>

#include "../EffectController.h"

extern Effect juggle;

namespace juggleNamespace
{
	extern void reset();
	extern void tick();
}

#endif