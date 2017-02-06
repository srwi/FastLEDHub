#ifndef _HARDWARE_H
#define _HARDWARE_H

#include <Arduino.h>
#include <Ticker.h>
#include "FastLED.h"

#include "EffectController.h"
#include "Fade.h"

#define MAX_BETTER_HUE 1535
#define NUM_LEDS 87 // Marly: 87
#define LIGHTSTRIP_PIN 5 // Pin 1 on NodeMCU; Pin 5 on Generic/Weemos D1 Mini
#define BUTTON_PIN 4

extern CRGB strip[NUM_LEDS];
extern uint8_t brightness;
extern uint8_t gammaCorrectedBrightness;

void initHardware();
CRGB betterHue(uint16_t fract1535, int16_t sat = -1, uint8_t val = 255);
uint8_t getPotiBrightness();
void setGammaCorrectedBrightness(uint8_t newBrightness);
void handleInput();

#endif