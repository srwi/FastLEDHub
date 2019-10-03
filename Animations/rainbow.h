#pragma once

class Rainbow : public Animation
{
public:
  using Animation::Animation;

  void reset()
  {
  }

  void loop()
  {
    // For beatsin88, BPM MUST BE SPECIFIED as a Q8.8 fixed-point value; e.g. 120BPM must be specified as 120*256 = 30720.
    uint16_t startHue = beatsin88(Config.speed * 256 / 23.2 + 50, 0, MAX_BETTER_HUE);
    uint16_t endHue = beatsin88(Config.speed * 256 / 14.4 + 50, 0, MAX_BETTER_HUE);

    for (int16_t i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = betterHue(startHue + (endHue - startHue) * i / NUM_LEDS, Config.saturation);
    }

    FastLED.show();
  }
};