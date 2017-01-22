#ifndef _CREMERAINBOW_H
#define _CREMERAINBOW_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern EffectConfiguration cremeRainbow;

namespace cremeRainbowNamespace
{
	extern void reset();
	extern void tick();
}

#endif