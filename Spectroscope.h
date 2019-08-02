#pragma once

#include <Arduino.h>

#include "Animation.h"
#include "Hardware.h"

void updateSpectroscope(uint8_t *arr, bool isSymmetrical);
void linearSpectroscope(uint8_t *arr);
void symmetricalSpectroscope(uint8_t *arr);