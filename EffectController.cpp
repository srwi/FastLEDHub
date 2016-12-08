#include "EffectController.h"

Ticker effectTicker;

bool canBeResumed = false;
bool effectRunning = false;
LinkedList<Effect> effectList;
uint8_t effectIndex = 0;
uint8_t speed = 0;

void initController()
{
	effectList.add({"Custom Color", customColor});
	effectList.add({"Gradient", gradient});
}

void begin(String newName, int8_t newSpeed)
{
	// Check if effect is already running
	if(effectRunning && effectList.get(effectIndex).name == newName)
		return;

	// Get effect index from name
	for(uint8_t i = 0; i < effectList.size(); i++)
	{
		if(effectList.get(i).name == newName)
		{
			effectIndex = i;
			break;
		}
	}

	// Save lastEffectIndex to config
	Config.last_effect = newName;
	Config.save();

	// Change speed if provided as parameter
	if(newSpeed >= 0)
		speed = newSpeed;

	// Start effect
	effectTicker.detach();
	FastLED.clear();
	FastLED.show();
	effectList.get(effectIndex).configuration.reset();
	attachTicker();
	canBeResumed = false;
	effectRunning = true;

	Serial.println("[EffectController] Started '" + Config.last_effect + "' effect.");
}

void cycleEffect()
{
	uint8_t nextEffectIndex = effectIndex;

	if(effectRunning)
	{
		nextEffectIndex++;
		nextEffectIndex %= effectList.size();
	}

	stop();
	begin(effectList.get(nextEffectIndex).name);
}

void stop()
{
	canBeResumed = false;
	effectRunning = false;
	effectTicker.detach();
	effectList.get(effectIndex).configuration.reset();
	FastLED.clear();
	FastLED.show();
}

void resume()
{
	if(!canBeResumed)
	{
		//effectList.get(effectIndex).configuration.reset();
		return;
	}

	attachTicker();
	canBeResumed = false;
	effectRunning = true;
}

void restart()
{
	stop();
	begin(effectList.get(effectIndex).name);
}

void pause()
{
	effectTicker.detach();
	canBeResumed = true;
	effectRunning = false;
}

void setSpeed(uint8_t newSpeed)
{
	speed = newSpeed;

	if(effectRunning)
	{
		effectTicker.detach();
		attachTicker();
	}
}

void attachTicker()
{
	uint16_t interval = effectList.get(effectIndex).configuration.intervalZeroOffset + speed * effectList.get(effectIndex).configuration.intervalStepSize;

	effectTicker.attach_ms(interval, effectList.get(effectIndex).configuration.tick);
}