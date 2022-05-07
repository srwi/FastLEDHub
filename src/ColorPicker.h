#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>

class ColorPicker
{
public:
    String name;
    CRGB value;
    String icon;

    ColorPicker(String _name, CRGB _value, String _icon = "eyedropper")
    {
        name = _name;
        value = _value;
        icon = _icon;
    }
};
