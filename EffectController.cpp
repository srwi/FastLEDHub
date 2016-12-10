#include "EffectController.h"

Ticker effectTicker;

bool canBeResumed = false;
bool effectRunning = false;
LinkedList<Effect> effectList;
uint8_t effectIndex = 0;

void initController()
{
	effectList.add({"Custom Color", customColor});
	effectList.add({"Gradient", gradient});
}

void begin(String name)
{
	// Check if effect is already running
	if(effectRunning && effectList.get(effectIndex).name == name)
		return;

	// Get effect index from name
	for(uint8_t i = 0; i < effectList.size(); i++)
	{
		if(effectList.get(i).name == name)
		{
			effectIndex = i;

			// Save last effect to config
			// Config.last_effect = name;
			// Config.save();

			break;
		}
	}

	// Start effect
	effectTicker.detach();
	FastLED.clear();
	FastLED.show();
	effectList.get(effectIndex).configuration.reset();
	attachTicker();
	canBeResumed = false;
	effectRunning = true;
	// Broadcast to websocket clients
	webSocket.broadcastTXT(String("start " + effectList.get(effectIndex).name).c_str());

	Serial.println("[EffectController] Started '" + effectList.get(effectIndex).name + "' effect.");
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
	FastLED.clear();
	FastLED.show();
	// Broadcast to websocket clients
	webSocket.broadcastTXT(String("stop").c_str());
}

void resume()
{
	if(!canBeResumed)
	{
		return;
	}

	attachTicker();
	canBeResumed = false;
	effectRunning = true;
	// Broadcast to websocket clients
	webSocket.broadcastTXT(String("start " + effectList.get(effectIndex).name).c_str());
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
	// Broadcast to websocket clients
	webSocket.broadcastTXT(String("pause " + effectList.get(effectIndex).name).c_str());
}

void setSpeed(uint8_t newSpeed)
{
	Config.speed = newSpeed;

	if(effectRunning)
	{
		effectTicker.detach();
		attachTicker();
	}
}

void toggle(String name)
{
	if(effectRunning && effectList.get(effectIndex).name == name)
	{
		pause();
	}
	else
	{
		begin(name);
	}
}

void attachTicker()
{
	uint16_t interval = effectList.get(effectIndex).configuration.intervalZeroOffset + Config.speed * effectList.get(effectIndex).configuration.intervalStepSize;

	effectTicker.attach_ms(interval, effectList.get(effectIndex).configuration.tick);
}