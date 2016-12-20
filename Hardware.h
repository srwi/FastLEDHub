#ifndef _HARDWARE_H
#define _HARDWARE_H

#include <Arduino.h>
#include <Ticker.h>
#include "FastLED.h"

#include "EffectController.h"
#include "Fade.h"

#define NUM_LEDS 87
#define LIGHTSTRIP_PIN 1 // corresponds to pin 5
#define BUTTON_PIN 4

extern CRGB strip[NUM_LEDS];
extern int16_t brightness;
extern uint8_t gammaCorrectedBrightness;

void initHardware();
int16_t getPotiBrightness();
void setGammaCorrectedBrightness(uint8_t newBrightness);
void handleInput();

#endif