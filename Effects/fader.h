#ifndef _FADER_H
#define _FADER_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern Effect fader;

namespace faderNamespace
{
	extern void reset();
	extern void tick();
}

#endif