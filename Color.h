#ifndef _COLOR_H
#define _COLOR_H

#include <Arduino.h>

#include "Types.h"
#include "Hardware.h"

extern rgb currentColor;

rgb toRGB(hsv input);
hsv toHSV(rgb input);
rgb rotateColor(rgb color, float amount);
hsv rotateColor(hsv color, float amount);
uint16_t byteToColorRange(uint8_t input);

#endif