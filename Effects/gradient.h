#ifndef _GRADIENT_H
#define _GRADIENT_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern EffectConfiguration gradient;

namespace gradientNamespace
{
	extern void reset();
	extern void tick();
}

#endif