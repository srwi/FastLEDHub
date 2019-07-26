#pragma once

class ColorFader : public Animation
{
  public:
    using Animation::Animation;

    uint16_t hue1535 = 0;

    void reset()
    {
      hue1535 = 0;
    }

    void loop()
    {
      if(hue1535 > 1535)
        hue1535 = 0;

      fill_solid(strip, NUM_LEDS, betterHue(hue1535, Config.saturation));
      hue1535++;

      delay(50);
    }
};
