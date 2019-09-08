#pragma once

class RgbWave : public Animation
{
public:
  using Animation::Animation;

  // Division will leave a small remainder but won't be noticable
  uint8_t ledDiv = MAX_BETTER_HUE / NUM_LEDS;
  uint16_t step;

  void reset()
  {
    step = 0;
  }

  void loop()
  {
    for (uint16_t i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = betterHue((ledDiv * i + step) % MAX_BETTER_HUE, Config.saturation);
    }

    step++;
    if (step == MAX_BETTER_HUE)
      step = 0;

    delay(5);
  }
};
