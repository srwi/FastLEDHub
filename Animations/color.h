#pragma once

// #include "Animation.h"

// class Animation;

class Color : public Animation
{
  public:
    using Animation::Animation;

    void reset() {}

    void loop()
    {
      for(int i=0; i<NUM_LEDS; i++)
      {
        strip[i] = strtol((const char *)&Config.color[0], NULL, 16);
      }

      // TODO: remove delay completely
      delay(10);
    }
};
