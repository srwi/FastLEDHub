#ifndef _CONFETTI_H
#define _CONFETTI_H

#include <Arduino.h>

#include "../EffectController.h"

extern Effect confetti;

namespace confettiNamespace
{
	extern void reset();
	extern void tick();
}

#endif