#ifndef _WAKEUP_H
#define _WAKEUP_H

#include <Arduino.h>
#include <Time.h>

#include "../Config.h"
#include "../EffectController.h"

extern Effect wakeup;

namespace wakeupNamespace
{
	extern void reset();
	extern void tick();
}

#endif