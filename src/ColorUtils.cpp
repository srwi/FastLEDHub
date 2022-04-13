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

String rgb2hex(CRGB rgb)
{
  String output = "";

  if (rgb.r == 0)
    output += "00";
  else
    output += (rgb.r < 16 ? "0" : "") + String(rgb.r, HEX);

  if (rgb.g == 0)
    output += "00";
  else
    output += (rgb.g < 16 ? "0" : "") + String(rgb.g, HEX);

  if (rgb.b == 0)
    output += "00";
  else
    output += (rgb.b < 16 ? "0" : "") + String(rgb.b, HEX);

  return output;
}

CRGB hex2rgb(String hex)
{
  return strtol(hex.c_str(), NULL, 16);
}