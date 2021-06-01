#include "FastLEDManager.h"

#include "Animation.h"
#include "ColorUtils.h"
#include "Config.h"
#include "Fade.h"
#include "SerialOut.h"
#include "Webserver.h"
#include "WebSocket.h"

#include <ESPEssentials.h>


FastLEDManagerClass::FastLEDManagerClass() :
  status(STOPPED),
  brightness10(1023),
  cycleButtonPushed(false),
  toggleButtonPushed(false),
  autostartHandled(false),
  filteredBrightness(128),
  potentiometerPin(-1),
  cycleButtonPin(-1),
  toggleButtonPin(-1)
{
}

void FastLEDManagerClass::initialize(uint16_t numberOfLeds)
{
  numLeds = numberOfLeds;
  leds = new CRGB[numLeds];
  hardwareLeds = new CRGB[numLeds];
  brightness10 = 1023;
  Config.initialize();
  if (WiFi.status() == WL_CONNECTED)
  {
    WebSocket::initialize();
    Webserver::initialize();
    Fade::initialize();
  }
}

void FastLEDManagerClass::enableCycleButton(uint8_t pin)
{
  inputTicker.attach_ms(FASTLEDMANAGER_INPUT_TICKER_INTERVAL, std::bind(&FastLEDManagerClass::handleInput, this));
  pinMode(pin, INPUT);
  cycleButtonPin = pin;
}

void FastLEDManagerClass::enableToggleButton(uint8_t pin)
{
  inputTicker.attach_ms(FASTLEDMANAGER_INPUT_TICKER_INTERVAL, std::bind(&FastLEDManagerClass::handleInput, this));
  pinMode(pin, INPUT);
  cycleButtonPin = pin;
}

void FastLEDManagerClass::enablePotentiometer(uint8_t pin)
{
  inputTicker.attach_ms(FASTLEDMANAGER_INPUT_TICKER_INTERVAL, std::bind(&FastLEDManagerClass::handleInput, this));
  pinMode(pin, INPUT);
  potentiometerPin = pin;
}

void FastLEDManagerClass::handle()
{
  if (!autostartHandled)
    autostart();

  if (status == RUNNING && currentAnimation)
    currentAnimation->loop();

  Fade::handle();
  WebSocket::socket.loop();
}

void FastLEDManagerClass::show(int16_t bright10)
{
  if (bright10 != -1)
    brightness10 = bright10;

  int16_t gammaCorrectedBrightness = round((float)brightness10 * brightness10 / 1023);
  uint8_t bright8 = gammaCorrectedBrightness / 4;
  uint8_t fract2 = bright8 != 255 ? gammaCorrectedBrightness % 4 : 0;

  for (uint16_t i = 0; i < numLeds; i++)
  {
    hardwareLeds[i] = leds[i];
    switch (fract2)
    {
      case 0:
        hardwareLeds[i].nscale8(bright8);
        break;
      case 2:
        if (i % 2)
        {
          hardwareLeds[i].nscale8(bright8);
        }
        else
        {
          hardwareLeds[i].nscale8(bright8 + 1);
        }
        break;
      case 1:
      case 3:
        if (i % 4 < fract2)
        {
          hardwareLeds[i].nscale8(bright8 + 1);
        }
        else
        {
          hardwareLeds[i].nscale8(bright8);
        }
        break;
    }
  }

  CFastLED::show();
}

void FastLEDManagerClass::clear(bool writeData)
{
  for(uint16_t i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB::Black;
  }

  if (writeData)
    show();
}

void FastLEDManagerClass::delay(uint16_t ms)
{
  unsigned long start = micros();
  while (micros() - start < 1000.0 * ms * pow(Config.speed - 255, 2) / 16384)
  {
    show();
    handleESPEssentials();
    Fade::handle();
    WebSocket::socket.loop();
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
  if (potentiometerPin >= 0 && Fade::currentFade == Fade::FadeMode::NONE)
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

  if (cycleButtonPin >= 0)
  {
    if (!digitalRead(cycleButtonPin) && !cycleButtonPushed)
    {
      Fade::stop();
      cycle();
      cycleButtonPushed = true;
    }
    else if (digitalRead(cycleButtonPin) && cycleButtonPushed)
    {
      cycleButtonPushed = false;
    }
  }

  if (toggleButtonPin >= 0)
  {
    if (!digitalRead(toggleButtonPin) && !toggleButtonPushed)
    {
      Fade::stop();
      toggle();
      toggleButtonPushed = true;
    }
    else if (digitalRead(toggleButtonPin) && toggleButtonPushed)
    {
      toggleButtonPushed = false;
    }
  }
}

void FastLEDManagerClass::autostart()
{
  if (Config.startupAnimation != "")
    begin(getAnimation(Config.startupAnimation));

  autostartHandled = true;
}

void FastLEDManagerClass::begin(Animation *animation)
{
  if (currentAnimation && currentAnimation->getName() == animation->getName())
    return;

  clear(true);

  status = RUNNING;
  currentAnimation = animation;
  currentAnimation->reset();

  WebSocket::broadcastStatus();
  PRINTLN("Started '" + currentAnimation->getName() + "'");
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
  clear(true);
  currentAnimation = NULL;

  WebSocket::broadcastStatus();
  PRINTLN("Stopped animation");
}

void FastLEDManagerClass::resume()
{
  if (status != PAUSED)
    return;

  status = RUNNING;

  WebSocket::broadcastStatus();
  PRINTLN("Resumed '" + currentAnimation->getName() + "'");
}

void FastLEDManagerClass::showColor(const struct CRGB &color, uint8_t scale)
{
  fill_solid(leds, numLeds, color);
  brightness10 = scale * 4 * 1.003;
}

void FastLEDManagerClass::restart()
{
  stop();
  begin(currentAnimation);

  PRINTLN("Restarted '" + currentAnimation->getName() + "'");
}

void FastLEDManagerClass::pause()
{
  status = PAUSED;

  WebSocket::broadcastStatus();
  PRINTLN("Paused '" + currentAnimation->getName() + "'");
}

void FastLEDManagerClass::toggle()
{
  if (status == RUNNING)
    pause();
  else
    resume();
}

void FastLEDManagerClass::toggle(Animation *animation)
{
  if (currentAnimation && currentAnimation == animation && status == RUNNING)
    pause();
  else if (currentAnimation && currentAnimation == animation && status == PAUSED)
    resume();
  else
    begin(animation);
}

FastLEDManagerClass FastLEDManager;
