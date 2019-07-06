#pragma once

class Nox : public Animation
{
	public:
		using Animation::Animation;
	
		void reset()
		{
			FastLED.clear();
		}

		void loop()
		{
			FastLED.clear();
		}
};
