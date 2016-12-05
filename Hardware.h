#ifndef _HARDWARE_H
#define _HARDWARE_H

#include <Arduino.h>

#include "EffectController.h"
#include "Tables/gamma_correction_table.h"

#define POTI_ZERO_OFFSET 500

#define POTENTIOMETER_RED 14
#define POTENTIOMETER_GREEN 12
#define POTENTIOMETER_BLUE 13

#define RED 4
#define GREEN 5
#define BLUE 16

#define PWM_FREQUENCY 200
#define COLOR_RANGE 4095

extern rgb potiColorBeforeEffectStart;
extern rgb currentPotiColor;

void initHardware();
void writeColor(rgb newColor);
void writeBlank();
void potiTick();
uint16_t getPoti(uint8_t i);

#endif