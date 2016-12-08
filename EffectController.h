#ifndef _EFFECTCONTROLLER_H
#define _EFFECTCONTROLLER_H

#include <Arduino.h>
#include <Ticker.h>
#include <LinkedList.h>
#include "FastLED.h"

#include "Config.h"
#include "Hardware.h"

typedef void (*func_ptr)();

struct EffectConfiguration
{
	func_ptr tick;
	func_ptr reset;
	uint16_t intervalZeroOffset;
	uint16_t intervalStepSize;
};

struct Effect
{
	String name;
	EffectConfiguration configuration;
};

#include "Effects/customColor.h"
#include "Effects/gradient.h"

void initController();
void begin(String newName, int8_t newSpeed = -1);
void stop();
void pause();
void restart();
void resume();
void cycleEffect();
void setSpeed(uint8_t speed);
void attachTicker();

#endif