#include <Arduino.h>

class Slider
{
public:
    const String name;
    const int16_t min;
    const int16_t max;
    int16_t value;
    const int16_t step;

    Slider(String _name,
           int16_t _min,
           int16_t _max,
           int16_t _default_value,
           int16_t _step) : name(_name),
                            min(_min),
                            max(_max),
                            value(_default_value),
                            step(_step)
    {
    }
};
