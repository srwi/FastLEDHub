#pragma once

#include <Arduino.h>

namespace Spectroscope
{

void updateSpectroscope(uint8_t *arr, bool isSymmetrical);
void linearSpectroscope(uint8_t *arr);
void symmetricalSpectroscope(uint8_t *arr);

} // namespace Spectroscope
