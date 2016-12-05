#ifndef _EFFECTCONTROLLER_H
#define _EFFECTCONTROLLER_H

#include <Arduino.h>
#include <Ticker.h>
#include "Types.h"
#include "Color.h"

typedef void (*func_ptr)();

struct Effect
{
	String name;
	func_ptr tick;
	func_ptr reset;
	uint16_t speedZeroOffset;
	uint16_t speedStepSize;
	uint16_t currentInterval;
};

extern bool effectRunning;

void begin(Effect &eff, float interval = 0);
void stop();
void pause();
void restart();
void resume();
void setInterval(uint16_t interval);
void setSpeed(uint16_t speed);

#endif