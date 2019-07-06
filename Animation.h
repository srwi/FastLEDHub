#pragma once

#include <Ticker.h>
#include <LinkedList.h>

#include "Config.h"
#include "WebSocket.h"
#include "Hardware.h"

enum AnimationStatus
{
	STOPPED = 0,
	PAUSED,
	RUNNING
};

class Animation
{
	public:
		Animation() = default;
		Animation(String _name, uint8_t _intervalZeroOffset, uint8_t _intervalStepSize);

		void begin();
		void toggle();
		void stop();
		void pause();
		void restart();
		void resume();

		virtual void loop() = 0;
		virtual void reset() = 0;

		static AnimationStatus getStatus();
		static String getCurrent();
		//static void cycleEffect();

	private:
		String name;
		int intervalZeroOffset;
		int intervalStepSize;

		static Ticker ticker;
		static Animation* current;
		static AnimationStatus status;

		void loopAndShow();
		void clearAndReset();
		void attachTicker();
};

// Include animations here:
#include "Animations/alternatingRainbow.h"
#include "Animations/confetti.h"
#include "Animations/cremeRainbow.h"
#include "Animations/customColor.h"
#include "Animations/customColor2.h"
#include "Animations/fader.h"
#include "Animations/fire.h"
#include "Animations/juggle.h"
#include "Animations/leftRightLeftRightLeft.h"
#include "Animations/nox.h"
#include "Animations/popFade.h"
#include "Animations/rbRainbow.h"
#include "Animations/rgbRainbow.h"

void registerAnimations();