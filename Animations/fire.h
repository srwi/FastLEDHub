#pragma once

class Fire : public Animation
{
	public:
		using Animation::Animation;

		byte heat[NUM_LEDS] = { 0 };
		uint8_t SPARKING = 120;
		uint8_t COOLING = 55;

		void reset()
		{
			heat[NUM_LEDS] = { 0 };
		}

		void loop()
		{
			// Step 1.  Cool down every cell a little
			for( int i = 0; i < NUM_LEDS; i++) {
				heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
			}
		
			// Step 2.  Heat from each cell drifts 'up' and diffuses a little
			for( int k= NUM_LEDS - 1; k >= 2; k--) {
				heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
			}
			
			// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
			if( random8() < SPARKING ) {
				int y = random8(7);
				heat[y] = qadd8( heat[y], random8(160,255) );
			}

			// Step 4.  Map from heat cells to LED colors
			for( int j = 0; j < NUM_LEDS; j++) {
				CRGB color = HeatColor( heat[j]);
				int pixelnumber;
				pixelnumber = j;
				strip[pixelnumber] = color;
			}
		}
};
