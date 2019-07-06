#include "Animation.h"

Ticker Animation::ticker;
Animation* Animation::current;
AnimationStatus Animation::status = STOPPED;
LinkedList<Animation*> animations;

Animation::Animation(String _name, uint8_t _intervalZeroOffset, uint8_t _intervalStepSize)
{
	name = _name;
	intervalZeroOffset = _intervalZeroOffset;
	intervalStepSize = _intervalStepSize;
}

void Animation::begin()
{
	clearAndReset();
	attachTicker();
	current = this;
	status = RUNNING;

	broadcastStatus();
	Serial.println("Started '" + name + "'");
}

void Animation::stop()
{
	if(status == STOPPED)
		return;

	ticker.detach();
	FastLED.clear();
	betterShow();
	status = STOPPED;

	broadcastStatus();
	Serial.println("Stopped '" + name + "'");
}

void Animation::resume()
{
	if(status != PAUSED)
		return;

	attachTicker();
	status = RUNNING;

	broadcastStatus();
	Serial.println("Resumed '" + name + "'");
}

void Animation::restart()
{
	stop();
	begin();

	Serial.println("Restarted '" + name + "'");
}

void Animation::pause()
{
	ticker.detach();
	status = PAUSED;

	broadcastStatus();
	Serial.println("Paused '" + name + "'");
}

void Animation::toggle()
{
	if(status == RUNNING)
		pause();
	else if(status == PAUSED)
		resume();
	else
		begin();
}

void Animation::loopAndShow()
{
	loop();
	betterShow();
}

void Animation::clearAndReset()
{
	FastLED.clear();
	betterShow();
	reset();
}

void Animation::attachTicker()
{
	ticker.attach_ms(intervalZeroOffset + Config.speed * intervalStepSize, [&]() { loopAndShow(); });
}

String Animation::getCurrent()
{
	return current->name;
}

AnimationStatus Animation::getStatus()
{
	return status;
}

void registerAnimations()
{
	animations.add(new AlternatingRainbow("Alternating Rainbow", 5, 5));
	animations.add(new Confetti("Confetti", 5, 1));
	animations.add(new CremeRainbow("Creme Rainbow", 10, 0));
	animations.add(new Color("Color", 5, 3));
	animations.add(new Color2("Color 2", 8, 3));
	animations.add(new ColorFader("Color Fader", 4, 5));
	animations.add(new Fire("Fire", 5, 2));
	animations.add(new Juggle("Juggle", 5, 0));
	animations.add(new LeftRightLeftRightLeft("Left Right Left Right Left", 5, 2));
	animations.add(new Nox("Nox", 25, 0));
	animations.add(new PopFade("Pop Fade", 10, 7));
	animations.add(new RbRainbow("RG Rainbow", 2, 2));
	animations.add(new RgbRainbow("RGB Rainbow", 3, 1));

	animations.get(0)->begin();
}