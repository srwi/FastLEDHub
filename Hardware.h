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

void initHardware();
void handleInput();

#endif