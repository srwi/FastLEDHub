#include "EffectController.h"

Ticker effectTicker;
EffectStatus status = STOPPED;
LinkedList<Effect> effectList;
uint8_t effectIndex = 0;

void initController()
{
	// Load saved color from Config
	
	effectList.add({"Farbe", customColor});
	effectList.add({"Nox", nox});
	effectList.add({"Farbfade", fader});
	effectList.add({"Feuerwehr", fire});
	effectList.add({"Schiffchen", leftRightLeftRightLeft});
	effectList.add({"Rainbow", rgbRainbow});
	effectList.add({"Huiii", rbRainbow});
	effectList.add({"Konfetti", confetti});
	effectList.add({"#borntoparty", popFade});
	effectList.add({"Chillimilli", alternatingRainbow});
	effectList.add({"Creme", cremeRainbow});
	effectList.add({"Zirkus Zirkus", juggle});
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
	Serial.println("[Effect] Started '" + effectList.get(effectIndex).name + "'");
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
	begin(effectList.get(effectIndex).name);
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
	uint16_t interval = effectList.get(effectIndex).configuration.intervalZeroOffset + Config.speed * effectList.get(effectIndex).configuration.intervalStepSize;
	effectTicker.attach_ms(interval, effectList.get(effectIndex).configuration.tick);
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