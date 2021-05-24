#pragma once

#include <Arduino.h>
#define FASTLED_INTERNAL
#include <FastLED.h>
#include <LinkedList.h>
#include <Ticker.h>


#define NUM_LEDS 100
#define FASTLEDMANAGER_INPUT_TICKER_INTERVAL 10

class Animation;
class ConfigClass;

enum class AnimationStatus
{
  STOPPED,
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
    void initialize();

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

    /// Clear all pixels. This function should be used in combination with
    /// FastLEDManager rather than FastLED.clear().
    void clear();

    /// Wait for a given amount of milliseconds while still calling
    /// show() periodically. Use this instead of the regular Arduino delay.
    /// @param ms Milliseconds
    void delay(uint16_t ms);

    void begin(Animation *animation);
    void toggle();
    void stop();
    void pause();
    void restart();
    void resume();

    // CLEDController &addLeds(CLEDController *pLed, struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset);

    /// List of all registered animations
    LinkedList<Animation*> animations;

    Animation *currentAnimation;
    AnimationStatus status = AnimationStatus::STOPPED;

    /// Current brightness value in 10bit
    int16_t brightness10;

    CRGB leds[NUM_LEDS];
    CRGB brightnessCorrectedLeds[NUM_LEDS];

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
