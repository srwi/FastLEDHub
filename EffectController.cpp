#include "EffectController.h"

Ticker effectTicker;
EffectStatus status = STOPPED;
LinkedList<Effect> effectList;
uint8_t effectIndex = 0;

void initController()
{
	effectList.add(customColor2);
	effectList.add(customColor);
	effectList.add(nox);
	effectList.add(fader);
	effectList.add(fire);
	effectList.add(leftRightLeftRightLeft);
	effectList.add(rgbRainbow);
	effectList.add(rbRainbow);
	effectList.add(confetti);
	effectList.add(popFade);
	effectList.add(alternatingRainbow);
	effectList.add(cremeRainbow);
	effectList.add(juggle);
}

void begin(String name)
{
	// Get effect index from name
	uint8_t index = 0;
	for(uint8_t i = 0; i < effectList.size(); i++)
	{
		if(effectList.get(i).name == name)
		{
			index = i;
			break;
		}
	}

	begin(index);
}

void begin(uint8_t index)
{
	if((status == RUNNING && effectIndex == index) || index >= effectList.size())
		return;

	effectIndex = index;

	// Reset previous effect
	clear();
	betterShow();
	effectList.get(index).reset();

	// Start effect
	attachTicker();
	status = RUNNING;

	broadcastStatus();
	Serial.println("[Effect] Started '" + effectList.get(effectIndex).name + "'");
}

void begin(Effect effect)
{
	begin(effect.name);
}

void cycleEffect()
{
	uint8_t index = effectIndex;

	if(status == RUNNING)
	{
		// Increment effect index
		index++;
		index %= effectList.size();
	}

	begin(index);
}

void stop()
{
	if(status == STOPPED)
		return;

	effectTicker.detach();
	clear();
	betterShow();
	status = STOPPED;

	broadcastStatus();
	Serial.println("[Effect] Stopped '" + effectList.get(effectIndex).name + "'");
}

void resume()
{
	if(status != PAUSED)
		return;

	attachTicker();
	status = RUNNING;

	broadcastStatus();
	Serial.println("[Effect] Resumed '" + effectList.get(effectIndex).name + "'");
}

void restart()
{
	stop();
	begin(effectIndex);
}

void pause()
{
	effectTicker.detach();
	status = PAUSED;

	broadcastStatus();
	Serial.println("[Effect] Paused '" + effectList.get(effectIndex).name + "'");
}

void setSpeed(uint8_t speed)
{
	Config.speed = speed;

	if(status == RUNNING)
		attachTicker();
}

void toggle(String name)
{
	if(status == RUNNING && effectList.get(effectIndex).name == name)
		pause();
	else if(status == PAUSED && effectList.get(effectIndex).name == name)
		resume();
	else
		begin(name);
}

void attachTicker()
{
	uint16_t interval = effectList.get(effectIndex).intervalZeroOffset + Config.speed * effectList.get(effectIndex).intervalStepSize;
	effectTicker.attach_ms(interval, effectList.get(effectIndex).tick);
}

String getStatus()
{
	if(status == RUNNING)
	{
		return "start " + effectList.get(effectIndex).name;
	}
	else if(status == PAUSED)
	{
		return "pause " + effectList.get(effectIndex).name;
	}
	else
	{
		return "stop";
	}
}