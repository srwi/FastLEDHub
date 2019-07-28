#include "Animation.h"

Animation* Animation::current;
AnimationStatus Animation::status = STOPPED;
LinkedList<Animation*> animations;
bool Animation::is_delaying = false;
Ticker Animation::delayTicker;

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
  current = this;

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
  if(status == RUNNING)
    pause();
  else if(status == PAUSED)
    resume();
  else
    begin();
}

String Animation::getName()
{
  return name;
}

Animation* Animation::getCurrent()
{
  return current;
}

AnimationStatus Animation::getStatus()
{
  return status;
}

bool Animation::isDelaying() 
{
  return is_delaying;
}

void Animation::delay(uint16_t t)
{
  is_delaying = true;
  delayTicker.attach_ms(t * (255-Config.speed)/128, [&](){ is_delaying = false; });
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
    if(animations.get(i)->getName() == Animation::getCurrent()->getName())
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