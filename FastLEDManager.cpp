#include "FastLEDManager.h"

#include "Animation.h"
#include "ColorUtils.h"
#include "Config.h"
#include "Fade.h"
#include "Webserver.h"
#include "WebSocket.h"

#include <ESPEssentials.h>


Ticker inputTicker;
ConfigClass *config;
bool buttonPushed = false;
bool autostartHandled = false;
float filteredBrightness = 128;
int16_t brightness10 = 1023;
uint8_t potiPin = -1;
uint8_t buttonPin = -1;
CRGB leds[NUM_LEDS];
CRGB brightnessCorrectedLeds[NUM_LEDS];
Animation *currentAnimation;
AnimationStatus status = STOPPED;


void FastLEDManagerClass::initialize()
{
  brightness10 = 1023;
  config = new ConfigClass();
  config->init();
  if (WiFi.status() == WL_CONNECTED)
  {
    WebSocket::initialize();
    Webserver::initialize();
    Fade::initialize(config);
  }
}

void FastLEDManagerClass::setButtonPin(uint8_t pin)
{
  // inputTicker.attach_ms(FASTLEDMANAGER_INPUT_TICKER_INTERVAL, handleInput);
  pinMode(pin, INPUT);
  buttonPin = pin;
}

void FastLEDManagerClass::setPotiPin(uint8_t pin)
{
  // inputTicker.attach_ms(FASTLEDMANAGER_INPUT_TICKER_INTERVAL, handleInput);
  potiPin = pin;
}

void FastLEDManagerClass::handle()
{
  if (!autostartHandled)
    autostart();

  if (status == RUNNING && currentAnimation)
    currentAnimation->loop();

  Fade::handle();
  WebSocket::webSocket.loop();
}

void FastLEDManagerClass::show(int16_t bright10)
{
  if (bright10 != -1)
    brightness10 = bright10;

  int16_t gammaCorrectedBrightness = round((float)brightness10 * brightness10 / 1023);
  uint8_t bright8 = gammaCorrectedBrightness / 4;
  uint8_t fract2 = bright8 != 255 ? gammaCorrectedBrightness % 4 : 0;

  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    brightnessCorrectedLeds[i] = leds[i];
    switch (fract2)
    {
      case 0:
        brightnessCorrectedLeds[i].nscale8(bright8);
        break;
      case 2:
        if (i % 2)
        {
          brightnessCorrectedLeds[i].nscale8(bright8);
        }
        else
        {
          brightnessCorrectedLeds[i].nscale8(bright8 + 1);
        }
        break;
      case 1:
      case 3:
        if (i % 4 < fract2)
        {
          brightnessCorrectedLeds[i].nscale8(bright8 + 1);
        }
        else
        {
          brightnessCorrectedLeds[i].nscale8(bright8);
        }
        break;
    }
  }

  CFastLED::show();
}

void FastLEDManagerClass::clear()
{
  for(uint8_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
  show();
}

void FastLEDManagerClass::delay(uint16_t ms)
{
  unsigned long start = micros();
  while (micros() - start < 1000.0 * ms * pow(config->speed - 255, 2) / 16384)
  {
    handleESPEssentials();
    Fade::handle();
    WebSocket::webSocket.loop();
  }
}

void FastLEDManagerClass::registerAnimation(Animation *animation)
{
  animations.add(animation);
}

Animation* FastLEDManagerClass::getAnimation(String name)
{
  for (uint8_t i = 0; i < animations.size(); i++)
  {
    Animation *a = animations.get(i);
    if (a->getName() == name)
      return a;
  }

  return NULL;
}

Animation* FastLEDManagerClass::getAnimation(uint8_t i)
{
  return (i < animations.size()) ? animations.get(i) : NULL;
}

void FastLEDManagerClass::handleInput()
{
  if (Fade::currentFade == Fade::FadeMode::NONE)
  {
    // Adjust the range slightly so low and high adc values
    // span the whole 10bit brightness range
    int16_t potiBrightness = (1023 - analogRead(A0) - 29) * 1.06;
    potiBrightness = constrain(potiBrightness, 0, 1023);

    // Low pass filter the potentiometer value to smoothen it out
    filteredBrightness = filteredBrightness - 0.01 * (filteredBrightness - potiBrightness);

    // Only set brightness if it's not near the filtered brightness value.
    // This will cause the actual brightness to lock in place once the potentiometer
    // hasn't been adjusted to prevent it from changing randomly for low quality
    // potentiometers.
    if (!(filteredBrightness - 1 < potiBrightness && potiBrightness < filteredBrightness + 1))
    {
      brightness10 = (float)potiBrightness * potiBrightness / 1023;
    }
  }

  // Push button
  if (!digitalRead(buttonPin) && !buttonPushed)
  {
    Fade::stop();
    cycle();
    buttonPushed = true;
  }
  else if (digitalRead(buttonPin) && buttonPushed)
  {
    buttonPushed = false;
  }
}

void FastLEDManagerClass::autostart()
{
  if (config->startupAnimation != "")
    begin(getAnimation(config->startupAnimation));

  autostartHandled = true;
}

void FastLEDManagerClass::begin(Animation *animation)
{
  if (currentAnimation && currentAnimation->getName() == animation->getName())
    return;

  clear();

  status = RUNNING;
  currentAnimation = animation;
  currentAnimation->reset();

  WebSocket::broadcastStatus();
  Serial.println("Started '" + currentAnimation->getName() + "'");
}

void FastLEDManagerClass::cycle()
{
  for (uint8_t i = 0; i < animations.size(); i++)
  {
    if (currentAnimation == getAnimation(i))
    {
      begin(getAnimation((i + 1) % animations.size()));
      return;
    }
  }
}

void FastLEDManagerClass::stop()
{
  if (status == STOPPED)
    return;

  status = STOPPED;
  clear();
  currentAnimation = NULL;

  WebSocket::broadcastStatus();
  Serial.println("Stopped '" + currentAnimation->getName() + "'");
}

void FastLEDManagerClass::resume()
{
  if (status != PAUSED)
    return;

  status = RUNNING;

  WebSocket::broadcastStatus();
  Serial.println("Resumed '" + currentAnimation->getName() + "'");
}

void FastLEDManagerClass::restart()
{
  stop();
  begin(currentAnimation);

  Serial.println("Restarted '" + currentAnimation->getName() + "'");
}

void FastLEDManagerClass::pause()
{
  status = PAUSED;

  WebSocket::broadcastStatus();
  Serial.println("Paused '" + currentAnimation->getName() + "'");
}

void FastLEDManagerClass::toggle()
{
  if (currentAnimation && status == RUNNING)
    pause();
  else
    resume();
}

FastLEDManagerClass FastLEDManager;
