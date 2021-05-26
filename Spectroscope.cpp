#include "Spectroscope.h"

#include "Animation.h"
#include "Fade.h"
#include "FastLEDManager.h"

#include <FastLED.h>


namespace Spectroscope
{

uint8_t spectroscopeData[22] = {0};
CRGB color_1, color_2 = CRGB::Red;

void updateSpectroscope(uint8_t *arr, bool isSymmetrical)
{
  Fade::stop();
  FastLEDManager.stop();

  // Spectrum
  for (uint8_t i = 0; i < 16; i++)
  {
    spectroscopeData[i] = arr[i];
  }
  // Colors
  color_1 = CRGB(arr[16], arr[17], arr[18]);
  color_2 = CRGB(arr[19], arr[20], arr[21]);

  uint16_t stripe_width = isSymmetrical ? FastLEDManager.numLeds / 32 : FastLEDManager.numLeds / 16;
  uint16_t current_led = 0;
  for (int i = 0; i < 16; i++)
  {
    for (int j = 0; j < stripe_width; j++)
    {
      FastLEDManager.leds[current_led].red = color_1.red * spectroscopeData[i] / 255 * (15 - i) / 15 + color_2.red * spectroscopeData[i] / 255 * i / 15;
      FastLEDManager.leds[current_led].green = color_1.green * spectroscopeData[i] / 255 * (15 - i) / 15 + color_2.green * spectroscopeData[i] / 255 * i / 15;
      FastLEDManager.leds[current_led].blue = color_1.blue * spectroscopeData[i] / 255 * (15 - i) / 15 + color_2.blue * spectroscopeData[i] / 255 * i / 15;
      // Copy to second half
      if (isSymmetrical)
      {
        FastLEDManager.leds[FastLEDManager.numLeds - current_led].red = FastLEDManager.leds[current_led].red;
        FastLEDManager.leds[FastLEDManager.numLeds - current_led].green = FastLEDManager.leds[current_led].green;
        FastLEDManager.leds[FastLEDManager.numLeds - current_led].blue = FastLEDManager.leds[current_led].blue;
      }
      current_led++;
    }
  }
  FastLEDManager.show();
}

} // namespace Spectroscope
