#ifndef _TYPES_H
#define _TYPES_H

struct rgb
{
	uint16_t red;
	uint16_t green;
	uint16_t blue;
	bool operator==(const rgb& a) const
	{
		return (green == a.green && red == a.red && blue == a.blue);
	}
};

struct hsv
{
	float hue;
	float saturation;
	float value;
};

struct myTime
{
	int8_t hour;
	int8_t minute;
};

struct Effect;

#endif