#pragma once

#include "Animation.h"
#include "ColorUtils.h"
#include "Config.h"

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <LinkedList.h>
#include <Ticker.h>

#define FASTLEDMANAGER_INPUT_TICKER_INTERVAL 10

class Animation;
class ConfigClass;

enum AnimationStatus
{
  STOPPED = 0,
  PAUSED,
  RUNNING
};

class FastLEDManagerClass : public CFastLED
{
public:
  /// Constructor
  FastLEDManagerClass();

  /// Handle every function provided by FastLEDManager. This method
  /// should be called in the main Arduino loop-function.
  void handle();

  /// Initialize FastLEDManager
  void initialize(uint16_t numLeds);

  /// Initialize cycle button. The button will be used to cycle through animations.
  /// @param pin Button pin
  void enableCycleButton(uint8_t pin);

  /// Initialize toggle button. The button will be used to pause/resume animations.
  /// @param pin Button pin
  void enableToggleButton(uint8_t pin);

  /// Initialize potentiometer pin. The potentiometer will be used to
  /// control the overall brightness.
  /// @param pin Potentiometer pin
  void enablePotentiometer(uint8_t pin);

  /// Register an animation instance to be managed by FastLEDManager
  /// @param animation Pointer to animation instance
  void registerAnimation(Animation *animation);

  /// Begin the next animation with respect to the currently running/paused
  /// animation. Use this to cycle through animations one by one.
  void cycle();

  /// Return a pointer to an animation by name
  /// @param name Animation name
  /// @return Pointer to animation
  Animation *getAnimation(String name);

  /// Return a pointer to an animation by index
  /// @param i Animation index
  /// @return Pointer to animation
  Animation *getAnimation(uint8_t i);

  /// Perform gamma correction on the brightness value and use four neighboring
  /// pixels to further increase the perceived brightness
  /// @param bright10 10 bit brightness value
  void show(int16_t bright10 = -1);

  /// Set all LEDs on the controller to a given color/scale
  /// @param color Color to set LEDs to
  /// @param scale Brightness scale to set LEDs to (0 - 255)
  void showColor(const struct CRGB &color, uint8_t scale);

  /// Clear all pixels. This function should be used in combination with
  /// FastLEDManager rather than FastLED.clear().
  /// @param writeData Wether to write out the data
  void clear(bool writeData = false);

  /// Clear all pixels. This function should be used in combination with
  /// FastLEDManager rather than FastLED.clear().
  void clearData() { clear(); };

  /// Wait for a given amount of milliseconds while still calling
  /// show() periodically. Use this instead of the regular Arduino delay.
  /// @param ms Milliseconds
  void delay(uint16_t ms);

  /// Begin an animation
  /// @param animation Animation pointer
  void begin(Animation *animation);

  /// Toggle animation playback. This method will toggle between RUNNING
  /// and PAUSED animation status. Calling this method will have no effect
  /// if the animation status is STOPPED.
  void toggle();

  /// Toggle a specific animation. If this animation is not currently RUNNING
  /// or PAUSED it will begin instead.
  /// @param animation Animation to begin when STOPPED
  void toggle(Animation *animation);

  /// Stops the current animation. The animation status will be STOPPED.
  void stop();

  /// Pauses the current animation. The animation status will be PAUSED.
  void pause();

  /// Restarts the current animation. This will effectively call the animation's
  /// setup method again.
  void restart();

  /// Resumes a paused animation. If the current animation status is not PAUSED
  /// calling this method will have no effect.
  void resume();

  /// List of all registered animations
  LinkedList<Animation*> animations;

  /// Pointer to current animation object
  Animation *currentAnimation;

  /// Current animation status
  AnimationStatus status = STOPPED;

  /// Current brightness value in 10bit
  int16_t brightness10;

  /// Array holding all led colors. This array should be used to set led colors.
  CRGB *leds;

  /// Array holding the actual led colors that get used by FastLED.
  /// This will be used internally by FastLEDManager and should only be
  /// used once in combination with FastLEDManager.addLeds(). DO NOT use this
  /// to change led colors within an animation. Use FastLEDManager.leds instead.
  CRGB *hardwareLeds;

  /// Number of registered LEDs
  uint16_t numLeds;

private:
  /// Handle all configured inputs. This will get called periodically.
  void handleInput();

  /// Start the configured autostart animation
  void autostart();

  bool cycleButtonPushed;
  bool toggleButtonPushed;
  bool autostartHandled;
  float filteredBrightness;
  Ticker inputTicker;
  uint8_t potentiometerPin;
  uint8_t cycleButtonPin;
  uint8_t toggleButtonPin;
};

extern FastLEDManagerClass FastLEDManager;
