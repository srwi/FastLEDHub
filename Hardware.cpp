#include "Hardware.h"

rgb potiColorBeforeEffectStart = { 0, 0, 0 };
rgb currentPotiColor = { 0, 0, 0 };
float poti_values[3];
Ticker potiTicker;

void initHardware()
{
	// Set outputs
	pinMode(POTENTIOMETER_RED, OUTPUT);
	pinMode(POTENTIOMETER_GREEN, OUTPUT);
	pinMode(POTENTIOMETER_BLUE, OUTPUT);
	pinMode(RED, OUTPUT);
	pinMode(GREEN, OUTPUT);
	pinMode(BLUE, OUTPUT);

	analogWriteFreq(PWM_FREQUENCY);
	analogWriteRange(COLOR_RANGE);

	potiTicker.attach_ms(5, potiTick);
}

void writeColor(rgb newColor)
{
	analogWrite(RED, pgm_read_word_near(gamma_correction_table + newColor.red));
	analogWrite(GREEN, pgm_read_word_near(gamma_correction_table + newColor.green));
	analogWrite(BLUE, pgm_read_word_near(gamma_correction_table + newColor.blue));

	currentColor = newColor;
}

void writeBlank()
{
	writeColor(rgb { 0, 0, 0 });
}

void potiTick()
{
	// Low-pass filter poti values
	float beta = 0.01;
	poti_values[0] = poti_values[0] - beta * (poti_values[0] - getPoti(0));
	poti_values[1] = poti_values[1] - beta * (poti_values[1] - getPoti(1));
	poti_values[2] = poti_values[2] - beta * (poti_values[2] - getPoti(2));
	currentPotiColor = { poti_values[0], poti_values[1], poti_values[2] };

	if(effectRunning)
	{
		// Check if colors differ noticably
		const uint8_t threshold = 20;
		bool colorsDifferNoticably = false;
		if(abs(currentPotiColor.red - potiColorBeforeEffectStart.red) > threshold)
			colorsDifferNoticably = true;
		else if(abs(currentPotiColor.green - potiColorBeforeEffectStart.green) > threshold)
			colorsDifferNoticably = true;
		else if(abs(currentPotiColor.blue - potiColorBeforeEffectStart.blue) > threshold)
			colorsDifferNoticably = true;

		if(colorsDifferNoticably)
		{
			// Stop currently running effect
			stop();
		}
	}
	else
	{
		// No effect running: use poti color
		writeColor(currentPotiColor);
	}
}

uint16_t getPoti(uint8_t i)
{
	switch(i)
	{
		case 0:
			digitalWrite(POTENTIOMETER_RED, 1);
			digitalWrite(POTENTIOMETER_GREEN, 0);
			digitalWrite(POTENTIOMETER_BLUE, 0);
		break;
		case 1:
			digitalWrite(POTENTIOMETER_RED, 0);
			digitalWrite(POTENTIOMETER_GREEN, 1);
			digitalWrite(POTENTIOMETER_BLUE, 0);
		break;
		case 2:
			digitalWrite(POTENTIOMETER_RED, 0);
			digitalWrite(POTENTIOMETER_GREEN, 0);
			digitalWrite(POTENTIOMETER_BLUE, 1);
		break;
	}

	long value = (analogRead(A0)+1)*(COLOR_RANGE+1)/1024 - 1;

	if(value < POTI_ZERO_OFFSET)
		return 0;

	if(value > COLOR_RANGE)
		return COLOR_RANGE;

	// Adjust range
	return (float)COLOR_RANGE/(COLOR_RANGE-POTI_ZERO_OFFSET)*value - POTI_ZERO_OFFSET*COLOR_RANGE/(COLOR_RANGE-POTI_ZERO_OFFSET);
}