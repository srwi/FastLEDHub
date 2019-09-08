#pragma once

#include <Ticker.h>
#include <LinkedList.h>

#include "Config.h"
#include "Hardware.h"
#include "WebSocket.h"

enum AnimationStatus
{
  STOPPED = 0,
  PAUSED,
  RUNNING
};

class Animation
{
  public:
    Animation() = default;
    Animation(String _name);

    void begin();
    void toggle();
    void stop();
    void pause();
    void restart();
    void resume();
    String getName();

    virtual void loop() = 0;
    virtual void reset() = 0;

    static void delay(uint16_t t);

  private:
    String name;

    static Ticker delayTicker;
};

extern LinkedList<Animation*> animations;
extern Animation* currentAnimation;
extern AnimationStatus status;
extern bool isDelaying;

void registerAnimation(Animation* animation);
void beginNextAnimation();
Animation* getAnimation(String name);
Animation* getAnimation(uint8_t i);