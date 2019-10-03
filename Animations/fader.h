#pragma once

class Fader : public Animation
{
public:
  using Animation::Animation;

  void reset()
  {
    hue = 0;
  }

  void loop()
  {
    if (hue > MAX_BETTER_HUE)
      hue = 0;

    fill_solid(leds, NUM_LEDS, betterHue(hue, Config.saturation));
    hue++;

    delay(15);

    FastLED.show();
  }

private:
  uint16_t hue = 0;
};
