#pragma once

#include "Animation.h"
#include "ColorUtils.h"
#include "ColorPicker.h"
#include "Config.h"
#include "Slider.h"

#include <Arduino.h>
#include <ESPEssentials.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <LinkedList.h>
#include <Ticker.h>

#define FASTLEDHUB_INPUT_TICKER_INTERVAL 10
#define SPECTRUM_LENGTH 16

enum AnimationStatus
{
  STOPPED = 0,
  PAUSED,
  RUNNING
};

class FastLEDHubClass : public CFastLED
{
public:
  /// Constructor
  FastLEDHubClass();

  /// Handle every function provided by FastLEDHub. This method
  /// should be called in the main Arduino loop-function.
  void handle();

  /// Initialize FastLEDHub
  /// @param projectName Project name
  /// @param enableGammaCorrection Enable brightness gamma correction (default: true)
  void initialize(const String &projectName, bool enableGammaCorrection = true);

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

  /// Register a slider instance which can be used as a dynamic animation parameter
  /// @param slider Slider instance
  void registerSlider(Slider *slider);

  /// Register a color picker instance which can be used as a dynamic animation parameter
  /// @param colorPicker Color picker instance
  void registerColorPicker(ColorPicker *colorPicker);

  /// Register an animation instance to be managed by FastLEDHub
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

  /// Return a pointer to a slider by name
  /// @param name Slider name
  /// @return Pointer to slider
  Slider *getSlider(String name);

  /// Return a pointer to a slider by index
  /// @param i Slider index
  /// @return Pointer to slider
  Slider *getSlider(uint8_t i);

  /// Return a pointer to a color picker by name
  /// @param name Color picker name
  /// @return Pointer to color picker
  ColorPicker *getColorPicker(String name);

  /// Return a pointer to a color picker by index
  /// @param i Color picker index
  /// @return Pointer to color picker
  ColorPicker *getColorPicker(uint8_t i);

  /// Check if all leds are turned off (i.e. either brightness is zero or
  /// all leds are black)
  /// @return True if all leds are dim
  bool isDim();

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

  /// Get the current animation status.
  /// @return Current animation status
  AnimationStatus getStatus();

  /// Set global animation speed
  /// @param speed Speed value (0 - 255)
  void setSpeed(uint8_t speed);

  /// Set global brightness value. This will apply gamma correction if it
  /// is enabled from the initialize method.
  /// @param brightness Brightness value
  void setBrightness(uint8_t brightness);

  /// Get current global brightness value.
  /// @return Brightness value
  uint8_t getBrightness();

  /// Get the name of the currently selected animation
  /// @return Current animation name
  String getCurrentAnimationName();

  /// Spectrum data byte array of length SPECTRUM_LENGTH
  uint8_t spectrumData[SPECTRUM_LENGTH];

  /// List of all registered animation pointers
  LinkedList<Animation *> animations;

  /// List of all registered slider pointers
  LinkedList<Slider *> sliders;

  /// List of all registered color pickers
  LinkedList<ColorPicker *> colorPickers;

private:
  /// Handle all configured inputs. This will get called periodically.
  void handleInput();

  /// Start the configured autostart animation
  void autostart();

  bool m_cycleButtonPushed;
  bool m_toggleButtonPushed;
  bool m_autostartHandled;
  bool m_gammaCorrectionEnabled;
  float m_filteredBrightness;
  Ticker m_inputTicker;
  Animation *m_currentAnimation;
  AnimationStatus m_status = STOPPED;
  uint8_t m_brightness;
  uint8_t m_speed;
  int8_t m_potentiometerPin;
  int8_t m_cycleButtonPin;
  int8_t m_toggleButtonPin;
};

extern FastLEDHubClass FastLEDHub;
