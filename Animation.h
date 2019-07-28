#pragma once

#include <Ticker.h>
#include <LinkedList.h>

#include "Config.h"
#include "WebSocket.h"
#include "Hardware.h"

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

    static Animation* getCurrent();
    static AnimationStatus getStatus();
    static void delay(uint16_t t);
    static bool isDelaying();

  private:
    String name;
    
    static Animation* current;
    static AnimationStatus status;
    static bool is_delaying;
    static Ticker delayTicker;
};

extern LinkedList<Animation*> animations;

void registerAnimation(Animation* animation);
void beginNextAnimation();
Animation* getAnimation(String name);
Animation* getAnimation(uint8_t i);