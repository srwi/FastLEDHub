#include "EffectController.h"

Ticker effectTicker;
Effect effect;
bool canBeResumed = false;
bool effectRunning = false;

void begin(Effect &eff, float interval)
{
	// Check if effect is already running
	if(effectRunning && eff.name == effect.name)
		return;

	// Copy new interval to effect struct if provided
	if(interval)
		eff.currentInterval = interval;

	effect = eff;

	effectTicker.detach();
	writeBlank();
	effect.reset();
	effectTicker.attach_ms(effect.currentInterval, effect.tick);

	potiColorBeforeEffectStart = currentPotiColor;
	canBeResumed = false;
	effectRunning = true;
}

void stop()
{
	canBeResumed = false;
	effectRunning = false;
	effectTicker.detach();
	effect.reset();
	writeBlank();
}

void resume()
{
	if(!canBeResumed)
	{
		Serial.println("[Effect] The effect can not be resumed because it is currently in stopped state.");
		return;
	}

	effectTicker.attach_ms(effect.currentInterval, effect.tick);
	potiColorBeforeEffectStart = currentPotiColor;
	canBeResumed = false;
	effectRunning = true;
}

void restart()
{
	stop();
	begin(effect);
}

void pause()
{
	effectTicker.detach();
	canBeResumed = true;
	effectRunning = false;
}

void setInterval(uint16_t interval)
{
	effect.currentInterval = interval;

	if(effectRunning)
	{
		effectTicker.detach();
		effectTicker.attach_ms(effect.currentInterval, effect.tick);
	}
}

void setSpeed(uint16_t speed)
{
	setInterval(effect.speedZeroOffset + effect.speedStepSize * speed);
}