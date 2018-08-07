#ifndef _RGBRAINBOW_H
#define _RGBRAINBOW_H

#include <Arduino.h>

#include "../EffectController.h"

extern Effect rgbRainbow;

namespace rgbRainbowNamespace
{
	extern void reset();
	extern void tick();
}

#endif