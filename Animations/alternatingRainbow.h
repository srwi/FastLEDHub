#pragma once

class AlternatingRainbow : public Animation
{
  public:
    using Animation::Animation;

    void reset()
    {
      
    }

    void loop()
    {
      // beatsin88 takes BMP in multiples of 256, so 120BPM = 120*256
      uint16_t startHue = beatsin88((15 - Config.speed) * 256/5, 0, 1535);
      uint16_t endHue = beatsin88((15 - Config.speed) * 256/2, 0, 1535);

      if (startHue < endHue)
      {
        for(int16_t i = 0; i < NUM_LEDS; i++)
        {
          strip[i] = betterHue(startHue + (endHue - startHue) * i/NUM_LEDS, Config.saturation);
        }
      }
      else
      {
        for(int16_t i = NUM_LEDS - 1; i >= 0; i--)
        {
          strip[i] = betterHue(startHue + (endHue - startHue) * i/NUM_LEDS, Config.saturation);
        }
      }

      delay(50);
    }
};