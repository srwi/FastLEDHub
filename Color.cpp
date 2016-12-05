#include "Color.h"

rgb currentColor = rgb { 0, 0, 0 };

hsv toHSV(rgb input)
{
	float r = input.red / COLOR_RANGE;
	float g = input.green / COLOR_RANGE;
	float b = input.blue / COLOR_RANGE;

	hsv output;

	float max = r;
	if (max < g) max = g;
	if (max < b) max = b;
	float min = r;
	if (min > g) min = g;
	if (min > b) min = b;
	
	output.hue = 0;
	if (max == min) output.hue = 0;
	else if (max == r) {
		output.hue = 60 * (g - b) / (max - min);
		if (output.hue < 0) output.hue += 360;
		if (output.hue >= 360) output.hue -= 360;
	} else if (max == g) {
		output.hue = 60 * (b - r) / (max - min) + 120;
	} else if (max == b) {
		output.hue = 60 * (r - g) / (max - min) + 240;
	}

	if (max == 0) output.saturation = 0;
	else output.saturation = 1 - (min / max);

	output.value = max;

	return output;
}

rgb toRGB(hsv input)
{
	if (input.hue < 0) input.hue = 0;
	if (input.hue > 359) input.hue = 359;
	if (input.saturation < 0) input.saturation = 0;
	if (input.saturation > 1) input.saturation = 100;
	if (input.value < 0) input.value = 0;
	if (input.value > 1) input.value = 100;

	float tmp = input.hue / 60.0;
	int hi = floor(tmp);
	float f = tmp - hi;
	float p = input.value * (1 - input.saturation);
	float q = input.value * (1 - f * input.saturation);
	float t = input.value * (1 - (1 - f) * input.saturation);

	rgb output;
	
	switch (hi)
	{
		case 0:
			output.red = input.value * COLOR_RANGE;
			output.green = t * COLOR_RANGE;
			output.blue = p * COLOR_RANGE;
			break;
		case 1:
			output.red = q * COLOR_RANGE;
			output.green = input.value * COLOR_RANGE;
			output.blue = p * COLOR_RANGE;
			break;
		case 2:
			output.red = p * COLOR_RANGE;
			output.green = input.value * COLOR_RANGE;
			output.blue = t * COLOR_RANGE;
			break;
		case 3:
			output.red = p * COLOR_RANGE;
			output.green = q * COLOR_RANGE;
			output.blue = input.value * COLOR_RANGE;
			break;
		case 4:
			output.red = t * COLOR_RANGE;
			output.green = p * COLOR_RANGE;
			output.blue = input.value * COLOR_RANGE;
			break;
		case 5:
			output.red = input.value * COLOR_RANGE;
			output.green = p * COLOR_RANGE;
			output.blue = q * COLOR_RANGE;
			break;
	}

	return output;
}

hsv rotateColor(hsv color, float amount)
{
	hsv output = hsv { color.hue + amount, color.saturation, color.value };

	while(output.hue >= 360) output.hue -= 360;
	while(output.hue < 0) output.hue += 360;

	return output;
}

rgb rotateColor(rgb color, float amount)
{
	return toRGB(rotateColor(toHSV(color), amount));
}

uint16_t byteToColorRange(uint8_t input)
{
	return (float)input / 255 * COLOR_RANGE;
}