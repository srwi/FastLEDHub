#pragma once

// This example requires the use of https://github.com/srwi/FastLEDHub_AudioViz to send
// spectrum data to the ESP32/ESP8266 running FastLEDHub.

#include <FastLEDHub.h>

#include <math.h>

extern CRGB leds[];

class Spectrogram : public Animation
{
public:
  using Animation::Animation;

  uint8_t interpolateSpectrum(uint16_t led)
  {
    float stepSize = (float)(SPECTRUM_LENGTH - 1) / ((float)FastLEDHub.size() - 1);
    float spectrumPosition = led * stepSize;
    int16_t spectrumFloorIndex = (int16_t)spectrumPosition % SPECTRUM_LENGTH;
    int16_t spectrumFloor = FastLEDHub.spectrumData[spectrumFloorIndex];
    int16_t spectrumCeil = FastLEDHub.spectrumData[(spectrumFloorIndex + 1) % SPECTRUM_LENGTH];
    uint8_t intensity = spectrumFloor + fmod(spectrumPosition, 1) * (spectrumCeil - spectrumFloor);
    return intensity;
  }

  void reset()
  {
  }

  void loop()
  {
    for (uint16_t i = 0; i < FastLEDHub.size(); i++)
    {
      CRGB color = CRGB::White;
      leds[i] = color.nscale8(interpolateSpectrum(i));
    }

    FastLEDHub.show();
  }
};
