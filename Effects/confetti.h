#ifndef _CONFETTI_H
#define _CONFETTI_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern EffectConfiguration confetti;

namespace confettiNamespace
{
	extern void reset();
	extern void tick();
}

#endif