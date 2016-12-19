#ifndef _JUGGLE_H
#define _JUGGLE_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern EffectConfiguration juggle;

namespace juggleNamespace
{
	extern void reset();
	extern void tick();
}

#endif