#pragma once

class ColorRider : public Animation
{
public:
  using Animation::Animation;

  void reset()
  {
    hue = 0;
    increaseLedIndex = false;
    currentLed = 0;
    scale = min(250, (int)((250.0 - 90.0) / 250.0 * NUM_LEDS + 90));
  }

  void loop()
  {
    leds[currentLed] = CHSV(hue++, Config.saturation, 255);
    for (int i = 0; i < NUM_LEDS; i++)
      leds[i].nscale8(scale);
    if (currentLed == NUM_LEDS - 1 || currentLed == 0)
      increaseLedIndex = !increaseLedIndex;
    currentLed += increaseLedIndex ? 1 : -1;

    delay(50);
  }

private:
  uint8_t hue, scale;
  int16_t currentLed;
  bool increaseLedIndex;
};
