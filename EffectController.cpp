#include "EffectController.h"

Ticker effectTicker;
EffectStatus status = STOPPED;
LinkedList<Effect> effectList;
uint8_t effectIndex = 0;

void initController()
{
	customColorNamespace::set(strtol((const char *)&Config.custom_color[0], NULL, 16));
	
	effectList.add({"Custom Color", customColor});
	effectList.add({"Nox", nox});
	effectList.add({"Fader", fader});
	effectList.add({"Fire", fire});
	effectList.add({"LeftRightLeftRightLeft", leftRightLeftRightLeft});
	effectList.add({"RGB Rainbow", rgbRainbow});
	effectList.add({"RB Rainbow", rbRainbow});
	effectList.add({"Confetti", confetti});
	effectList.add({"Pop Fade", popFade});
	effectList.add({"Alternating Rainbow", alternatingRainbow});
	effectList.add({"Juggle", juggle});
}

void begin(String name)
{
	// Check if effect is already running
	if(status == RUNNING && effectList.get(effectIndex).name == name)
		return;

	// Get effect index from name
	for(uint8_t i = 0; i < effectList.size(); i++)
	{
		if(effectList.get(i).name == name)
		{
			effectIndex = i;
			break;
		}
	}

	// Reset previous effect
	FastLED.clear();
	FastLED.show();
	effectList.get(effectIndex).configuration.reset();

	// Start effect
	attachTicker();
	status = RUNNING;

	broadcastStatus();
}

void cycleEffect()
{
	uint8_t nextEffectIndex = effectIndex;

	if(status == RUNNING)
	{
		// Increment effect index
		nextEffectIndex++;
		nextEffectIndex %= effectList.size();
	}

	begin(effectList.get(nextEffectIndex).name);
}

void stop()
{
	effectTicker.detach();
	FastLED.clear();
	FastLED.show();
	status = STOPPED;

	broadcastStatus();
}

void resume()
{
	if(status != PAUSED)
		return;

	attachTicker();
	status = RUNNING;

	broadcastStatus();
}

void restart()
{
	stop();
	begin(effectList.get(effectIndex).name);
}

void pause()
{
	effectTicker.detach();
	status = PAUSED;

	broadcastStatus();
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
	else
		begin(name);
}

void attachTicker()
{
	uint16_t interval = effectList.get(effectIndex).configuration.intervalZeroOffset + Config.speed * effectList.get(effectIndex).configuration.intervalStepSize;
	effectTicker.attach_ms(interval, effectList.get(effectIndex).configuration.tick);
}

void broadcastStatus()
{
	if(status == RUNNING)
	{
		webSocket.broadcastTXT(String("start " + effectList.get(effectIndex).name).c_str());
		Serial.println("[Effect] Started '" + effectList.get(effectIndex).name + "'");
	}
	else if(status == PAUSED)
	{
		webSocket.broadcastTXT(String("pause " + effectList.get(effectIndex).name).c_str());
		Serial.println("[Effect] Paused '" + effectList.get(effectIndex).name + "'");
	}
	else if(status == STOPPED)
	{
		webSocket.broadcastTXT(String("stop").c_str());
		Serial.println("[Effect] Stopped '" + effectList.get(effectIndex).name + "'");
	}
}