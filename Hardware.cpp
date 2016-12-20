#include "Hardware.h"

Ticker inputTicker;
bool buttonPushed = false;
float filteredBrightness = 255/2;
uint8_t brightness = 255;
uint8_t gammaCorrectedBrightness = 255;
CRGB strip[NUM_LEDS];

const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };

void initHardware()
{
	pinMode(BUTTON_PIN, INPUT);
	inputTicker.attach_ms(10, handleInput);
	FastLED.addLeds<WS2812B, LIGHTSTRIP_PIN, GRB>(strip, NUM_LEDS);
	FastLED.setDither(1);
	FastLED.setTemperature(Tungsten100W);
}

void setGammaCorrectedBrightness(uint8_t newBrightness)
{
	brightness = newBrightness;
	gammaCorrectedBrightness = pgm_read_byte(&gamma8[newBrightness]);
	FastLED.setBrightness(gammaCorrectedBrightness);
}

uint8_t getPotiBrightness()
{
	int16_t value = (((1023 - analogRead(A0)) >> 2) - 7) * 1.06;

	if(value > 255)
		value = 255;
	if(value < 0)
		value = 0;

	return value;
}

void handleInput()
{
	if(!currentFade)
	{
		// Adjust brightness calculation if needed
		uint8_t potiBrightness = getPotiBrightness();

		filteredBrightness = filteredBrightness - 0.01 * (filteredBrightness - potiBrightness);

		// Only set brightness if it's not near the filtered brightness value which will lag behind
		if(!(filteredBrightness - 1 < potiBrightness && potiBrightness < filteredBrightness + 1))
		{
			//FastLED.setBrightness(brightness);
			setGammaCorrectedBrightness(potiBrightness);
			Serial.println("Set brightness");
		}
	}

	// Push button
	if(!digitalRead(BUTTON_PIN) && !buttonPushed)
	{
		// button pushed
		stopFade();
		cycleEffect();

		buttonPushed = true;
	}
	else if(digitalRead(BUTTON_PIN) && buttonPushed)
	{
		// button released

		buttonPushed = false;
	}
}