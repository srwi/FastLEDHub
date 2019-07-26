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
  delayTicker.attach_ms(t * Config.speed, [&](){ is_delaying = false; });
}

void registerAnimations()
{
  animations.add(new AlternatingRainbow("Alternating Rainbow"));
  animations.add(new Confetti("Confetti"));
  animations.add(new CremeRainbow("Creme Rainbow"));
  animations.add(new Color("Color"));
  animations.add(new Color2("Color 2"));
  animations.add(new ColorFader("Color Fader"));
  animations.add(new Fire("Fire"));
  animations.add(new Juggle("Juggle"));
  animations.add(new LeftRightLeftRightLeft("Left Right Left Right Left"));
  animations.add(new PopFade("Pop Fade"));
  animations.add(new RbRainbow("RG Rainbow"));
  animations.add(new RgbRainbow("RGB Rainbow"));

  animations.get(0)->begin();
}