#ifndef _EFFECTCONTROLLER_H
#define _EFFECTCONTROLLER_H

#include <Arduino.h>
#include <Ticker.h>
#include <LinkedList.h>
#include "FastLED.h"

#include "Config.h"
#include "Hardware.h"
#include "WebSocket.h"

typedef void (*func_ptr)();

struct Effect
{
	String name;
	func_ptr tick;
	func_ptr reset;
	uint16_t intervalZeroOffset;
	uint16_t intervalStepSize;
};

enum EffectStatus
{
	STOPPED = 0,
	PAUSED,
	RUNNING
};

#include "Effects/customColor.h"
#include "Effects/customColor2.h"
#include "Effects/nox.h"
#include "Effects/fader.h"
#include "Effects/fire.h"
#include "Effects/leftRightLeftRightLeft.h"
#include "Effects/rgbRainbow.h"
#include "Effects/rbRainbow.h"
#include "Effects/confetti.h"
#include "Effects/popFade.h"
#include "Effects/alternatingRainbow.h"
#include "Effects/cremeRainbow.h"
#include "Effects/juggle.h"
#include "Effects/sunrise.h"

extern EffectStatus status;
extern uint8_t effectIndex;
extern LinkedList<Effect> effectList;

void initController();
void begin(String name);
void begin(uint8_t index);
void begin(Effect effect);
void toggle(String name);
void stop();
void pause();
void restart();
void resume();
void cycleEffect();
void setSpeed(uint8_t speed);
void attachTicker();
String getStatus();

#endif