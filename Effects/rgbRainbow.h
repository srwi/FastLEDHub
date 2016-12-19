#ifndef _RGBRAINBOW_H
#define _RGBRAINBOW_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern EffectConfiguration rgbRainbow;

namespace rgbRainbowNamespace
{
	extern void reset();
	extern void tick();
}

#endif