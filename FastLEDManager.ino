#include "FastLEDManager.h"

#include "Animations/color.h"
#include "Animations/rbWave.h"
#include "Animations/rgbWave.h"

#include <ESPEssentials.h>

#define NUM_LEDS 100
#define LIGHTSTRIP_PIN 5

template <uint8_t DATA_PIN, EOrder RGB_ORDER = RGB, int WAIT_TIME = 5>
class WS2812Controller800Khz_noflicker : public ClocklessController<DATA_PIN, C_NS(250), C_NS(625), C_NS(375), RGB_ORDER, 0, false, WAIT_TIME> {};
template<uint8_t DATA_PIN, EOrder RGB_ORDER>
class WS2812B_noflicker : public WS2812Controller800Khz_noflicker<DATA_PIN, RGB_ORDER> {};

void setup()
{
  initESPEssentials("Lightstrip");

  FastLEDManager.initialize(NUM_LEDS);
  FastLEDManager.addLeds<WS2812B_noflicker, LIGHTSTRIP_PIN>(FastLEDManager.hardwareLeds, NUM_LEDS);
  FastLEDManager.registerAnimation(new Color("Color"));
  FastLEDManager.registerAnimation(new RbWave("RB Wave"));
  FastLEDManager.registerAnimation(new RgbWave("RGB Wave"));

  // FastLEDManager.enableCycleButton(4);
  // FastLEDManager.enablePotentiometer(5);

  FastLEDManager.setDither(0);
  FastLEDManager.setTemperature(Tungsten100W);
  FastLEDManager.setBrightness(255);
}

void loop()
{
  handleESPEssentials();
  FastLEDManager.handle();
}
