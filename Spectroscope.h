#ifndef _SPECTROSCOPE_H
#define _SPECTROSCOPE_H

#include <Arduino.h>

#include "Hardware.h"

void updateSpectroscope(uint8_t *arr, bool isSymmetrical);
void linearSpectroscope(uint8_t *arr);
void symmetricalSpectroscope(uint8_t *arr);

#endif