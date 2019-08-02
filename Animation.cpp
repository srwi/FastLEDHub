#include "Animation.h"

Ticker Animation::delayTicker;
LinkedList<Animation*> animations;
Animation* currentAnimation;
AnimationStatus status = STOPPED;
bool isDelaying = false;

Animation::Animation(String _name)
{
  name = _name;
}

void Animation::begin()
{
  FastLED.clear();
  betterShow();

  reset();
  status = RUNNING;
  currentAnimation = this;

  broadcastStatus();
  Serial.println("Started '" + name + "'");
}

void Animation::stop()
{
  if(status == STOPPED)
    return;

  status = STOPPED;
  FastLED.clear();
  betterShow();

  broadcastStatus();
  Serial.println("Stopped '" + name + "'");
}

void Animation::resume()
{
  if(status != PAUSED)
    return;

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
  status = PAUSED;

  broadcastStatus();
  Serial.println("Paused '" + name + "'");
}

void Animation::toggle()
{
  if(currentAnimation->getName() == name && status == RUNNING)
    pause();
  else if(currentAnimation->getName() == name && status == PAUSED)
    resume();
  else
    begin();
}

String Animation::getName()
{
  return name;
}

void Animation::delay(uint16_t t)
{
  isDelaying = true;
  delayTicker.attach_ms(t * (255-Config.speed)/128, [&](){ isDelaying = false; });
}

void registerAnimation(Animation* animation)
{
  animations.add(animation);
}

Animation* getAnimation(String name)
{
  for(uint8_t i = 0; i < animations.size(); i++)
  {
    Animation* a = animations.get(i);
    if(a->getName() == name)
      return a;
  }

  return NULL;
}

Animation* getAnimation(uint8_t i)
{
  return (animations.size() - 1 <= i) ? animations.get(i) : NULL;
}

void beginNextAnimation()
{
  for(uint8_t i = 0; i < animations.size(); i++)
  {
    if(animations.get(i)->getName() == currentAnimation->getName())
    {
      uint8_t nextAnimationIndex = i + 1;
      if(nextAnimationIndex < animations.size())
      {
        getAnimation(nextAnimationIndex)->begin();
      }
      else
      {
        getAnimation(0)->begin();
      }
    }
  }
}