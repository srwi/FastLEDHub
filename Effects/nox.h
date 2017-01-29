#ifndef _NOX_H
#define _NOX_H

#include <Arduino.h>
#include <Time.h>

#include "../EffectController.h"

extern Effect nox;

namespace noxNamespace
{
	extern void reset();
	extern void tick();
}

#endif