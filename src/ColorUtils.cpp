#include "ColorUtils.h"


CRGB hsv2rgb_smooth(uint16_t hue, uint8_t sat, uint8_t val)
{
  uint8_t fractHue = hue / 6;
  uint8_t fractBlend = hue % 6;

  CRGB leftColor, rightColor;
  hsv2rgb_rainbow(CHSV(fractHue, sat, val), leftColor);
  hsv2rgb_rainbow(CHSV(fractHue + 1, sat, val), rightColor);

  return blend(leftColor, rightColor, 42 * fractBlend);
}

String rgb2hex(uint8_t r, uint8_t g, uint8_t b)
{
  String output = "";

  if (r == 0)
    output += "00";
  else
    output += (r < 16 ? "0" : "") + String(r, HEX);

  if (g == 0)
    output += "00";
  else
    output += (g < 16 ? "0" : "") + String(g, HEX);

  if (b == 0)
    output += "00";
  else
    output += (b < 16 ? "0" : "") + String(b, HEX);

  return output;
}
