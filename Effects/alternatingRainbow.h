#ifndef _ALTERNATINGRAINBOW_H
#define _ALTERNATINGRAINBOW_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern EffectConfiguration alternatingRainbow;

namespace alternatingRainbowNamespace
{
	extern void reset();
	extern void tick();
}

#endif