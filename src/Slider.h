#include <Arduino.h>

class Slider
{
public:
    String name;
    int16_t min;
    int16_t max;
    int16_t value;
    int16_t step;
    String icon;

    Slider(String _name, int16_t _min, int16_t _max, int16_t _default_value, int16_t _step, String _icon = "sliders")
    {
        name = _name;
        min = _min;
        max = _max;
        value = _default_value;
        step = _step;
        icon = _icon;
    }
};
