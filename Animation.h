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
    static void delay(uint16_t t);
    static AnimationStatus getStatus();
    static bool isDelaying();

  private:
    String name;
    
    static Animation* current;
    static AnimationStatus status;
    static bool is_delaying;
    static Ticker delayTicker;
};

// Include animations here:
#include "Animations/alternatingRainbow.h"
#include "Animations/confetti.h"
#include "Animations/cremeRainbow.h"
#include "Animations/customColor.h"
#include "Animations/customColor2.h"
#include "Animations/fader.h"
#include "Animations/fire.h"
#include "Animations/juggle.h"
#include "Animations/leftRightLeftRightLeft.h"
#include "Animations/popFade.h"
#include "Animations/rbRainbow.h"
#include "Animations/rgbRainbow.h"

void registerAnimations();