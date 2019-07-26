#pragma once

class CremeRainbow : public Animation
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
      CRGB startColor = betterHue(startHue, Config.saturation);
      uint16_t endHue = beatsin88((15 - Config.speed) * 256/2, 0, 1535);
      CRGB endColor = betterHue(endHue, Config.saturation);

      if (startHue < endHue)
      {
        for(uint16_t i = 0; i < NUM_LEDS; i++)
        {
          strip[i] = blend(startColor, endColor, 255 * i/NUM_LEDS);
        }
      }
      else
      {
        for(uint16_t i = 0; i < NUM_LEDS; i++)
        {
          strip[i] = blend(endColor, startColor, 255 * i/NUM_LEDS);
        }
      } 

      delay(50);  
    }
};
