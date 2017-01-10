#ifndef _HARDWARE_H
#define _HARDWARE_H

#include <Arduino.h>
#include <Ticker.h>
#include "FastLED.h"

#include "EffectController.h"
#include "Fade.h"

#define NUM_LEDS 201 // Marly: 87
#define LIGHTSTRIP_PIN 5 // Pin 1 on NodeMCU; Pin 5 on Generic/Weemos D1 Mini
#define BUTTON_PIN 4

extern CRGB strip[NUM_LEDS];
extern uint8_t brightness;
extern uint8_t gammaCorrectedBrightness;

void initHardware();
uint8_t getPotiBrightness();
void setGammaCorrectedBrightness(uint8_t newBrightness);
void handleInput();

#endif