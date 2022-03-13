#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

#define HSV2RGB_SMOOTH_RANGE 1535


/// Convert a hue, saturation and value to RGB by using a higher resolution
/// of 0 - 1535 which allows for very smooth hue transitions without
/// noticeable steps. The number 1535 is also defined by HSV2RGB_SMOOTH_RANGE.
/// @param hue hue value (0 - 1535)
/// @param sat saturation value (0 - 255)
/// @param val color value (0 - 255)
/// @return CRGB color
CRGB hsv2rgb_smooth(uint16_t hue, uint8_t sat, uint8_t val);

/// Convert RGB color to Hex string representation
/// @param r Red
/// @param g Green
/// @param b Blue
/// @return Hex string representation
String rgb2hex(uint8_t r, uint8_t g, uint8_t b);
