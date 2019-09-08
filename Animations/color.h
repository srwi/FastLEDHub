#pragma once

class Color : public Animation
{
public:
  using Animation::Animation;

  void reset()
  {
    for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = strtol((const char *)&Config.color[0], NULL, 16);
    }
  }

  void loop()
  {
  }
};
