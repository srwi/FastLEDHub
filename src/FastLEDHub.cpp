#include "FastLEDHub.h"

#include "Fade.h"
#include "SerialOut.h"
#include "FWebserver.h"
#include "WebSocket.h"

FastLEDHubClass::FastLEDHubClass() : m_cycleButtonPushed(false),
                                     m_toggleButtonPushed(false),
                                     m_autostartHandled(false),
                                     m_gammaCorrectionEnabled(false),
                                     m_filteredBrightness(128),
                                     m_status(STOPPED),
                                     m_brightness(255),
                                     m_speed(255),
                                     m_potentiometerPin(-1),
                                     m_cycleButtonPin(-1),
                                     m_toggleButtonPin(-1)
{
}

void FastLEDHubClass::initialize(const String &projectName, bool enableGammaCorrection)
{
  m_gammaCorrectionEnabled = enableGammaCorrection;
  ESPEssentials::init(projectName);
  Config.initialize();
  if (WiFi.status() == WL_CONNECTED)
  {
    WebSocket::initialize(projectName);
    Webserver::initialize();
    Fade::initialize();
  }

  if (Config.sliderValues.size() >= 2)
  {
    m_speed = Config.sliderValues.get(1);
    m_brightness = Config.sliderValues.get(0);
    setBrightness(m_brightness);
  }

  registerSlider(new Slider("Brightness", 0, 255, 255, 1, "brightness-high"));
  registerSlider(new Slider("Speed", 0, 255, 127, 1, "speedometer"));
}

void FastLEDHubClass::enableCycleButton(uint8_t pin)
{
  m_inputTicker.attach_ms(FASTLEDHUB_INPUT_TICKER_INTERVAL, +[](FastLEDHubClass* t) { t->handleInput(); }, this);
  pinMode(pin, INPUT);
  m_cycleButtonPin = pin;
}

void FastLEDHubClass::enableToggleButton(uint8_t pin)
{
  m_inputTicker.attach_ms(FASTLEDHUB_INPUT_TICKER_INTERVAL, +[](FastLEDHubClass* t) { t->handleInput(); }, this);
  pinMode(pin, INPUT);
  m_toggleButtonPin = pin;
}

void FastLEDHubClass::enablePotentiometer(uint8_t pin)
{
  m_inputTicker.attach_ms(FASTLEDHUB_INPUT_TICKER_INTERVAL, +[](FastLEDHubClass* t) { t->handleInput(); }, this);
  pinMode(pin, INPUT);
  m_potentiometerPin = pin;
}

void FastLEDHubClass::handle()
{
  ESPEssentials::handle();

  if (ESPEssentials::OTA.isBusy())
    return;

  if (!m_autostartHandled)
    autostart();

  if (m_status == RUNNING && m_currentAnimation)
    m_currentAnimation->loop();

  Fade::handle();
  WebSocket::handle();
}

bool FastLEDHubClass::isDim()
{
  if (getBrightness() == 0)
    return true;

  for (uint16_t i = 0; i < FastLEDHub.count(); ++i)
  {
    for (uint16_t j = 0; j < FastLEDHub.size(); ++j)
    {
      if (FastLEDHub[i].leds()[j] != CRGB(0, 0, 0))
      {
        return false;
      }
    }
  }

  return true;
}

void FastLEDHubClass::setBrightness(uint8_t brightness)
{
  m_brightness = brightness;

  if (m_gammaCorrectionEnabled)
    brightness = round((float)brightness * brightness / 255.);

  CFastLED::setBrightness(brightness);
}

uint8_t FastLEDHubClass::getBrightness()
{
  return m_brightness;
}

void FastLEDHubClass::delay(uint16_t ms)
{
  unsigned long start = micros();
  while (micros() - start < 1000.0 * ms * pow((m_speed - 255) / 128.0, 2))
  {
    ESPEssentials::handle();
    Fade::handle();
    WebSocket::handle();
  }
}

void FastLEDHubClass::registerSlider(Slider *slider)
{
  if (Config.sliderValues.size() > sliders.size())
  {
    slider->value = Config.sliderValues.get(sliders.size());
  }
  else
  {
    Config.sliderValues.add(slider->value);
  }

  sliders.add(slider);
}

void FastLEDHubClass::registerColorPicker(ColorPicker *colorPicker)
{
  if (Config.colorPickerValues.size() > colorPickers.size())
  {
    colorPicker->value = Config.colorPickerValues.get(colorPickers.size());
  }
  else
  {
    Config.colorPickerValues.add(colorPicker->value);
  }

  colorPickers.add(colorPicker);
}

void FastLEDHubClass::registerAnimation(Animation *animation)
{
  animations.add(animation);
}

Animation *FastLEDHubClass::getAnimation(String name)
{
  for (uint8_t i = 0; i < animations.size(); i++)
  {
    Animation *a = animations.get(i);
    if (a->getName() == name)
      return a;
  }

  return NULL;
}

Animation *FastLEDHubClass::getAnimation(uint8_t i)
{
  return (i < animations.size()) ? animations.get(i) : NULL;
}

Slider *FastLEDHubClass::getSlider(String name)
{
  for (uint8_t i = 0; i < sliders.size(); i++)
  {
    Slider *a = sliders.get(i);
    if (a->name == name)
      return a;
  }

  return NULL;
}

Slider *FastLEDHubClass::getSlider(uint8_t i)
{
  return (i < sliders.size()) ? sliders.get(i) : NULL;
}

ColorPicker *FastLEDHubClass::getColorPicker(String name)
{
  for (uint8_t i = 0; i < colorPickers.size(); i++)
  {
    ColorPicker *a = colorPickers.get(i);
    if (a->name == name)
      return a;
  }

  return NULL;
}

ColorPicker *FastLEDHubClass::getColorPicker(uint8_t i)
{
  return (i < colorPickers.size()) ? colorPickers.get(i) : NULL;
}

void FastLEDHubClass::handleInput()
{
  if (m_potentiometerPin >= 0 && Fade::getMode() == Fade::FadeMode::NONE)
  {
    // Adjust the range slightly so low and high adc values
    // span the whole 10bit brightness range
    int16_t potiBrightness = (1023 - analogRead(m_potentiometerPin) - 29) * 1.06;
    potiBrightness = constrain(potiBrightness, 0, 1023);

    // Low pass filter the potentiometer value to smoothen it out
    m_filteredBrightness = m_filteredBrightness - 0.01 * (m_filteredBrightness - potiBrightness);

    // Only set brightness if it's not near the filtered brightness value.
    // This will cause the actual brightness to lock in place once the potentiometer
    // hasn't been adjusted to prevent it from changing randomly for low quality
    // potentiometers.
    if (!(m_filteredBrightness - 1 < potiBrightness && potiBrightness < m_filteredBrightness + 1))
    {
      uint16_t brightness = (float)potiBrightness * potiBrightness / 1023 / 4;
      setBrightness(brightness);
    }
  }

  if (m_cycleButtonPin >= 0)
  {
    if (!digitalRead(m_cycleButtonPin) && !m_cycleButtonPushed)
    {
      Fade::stop();
      cycle();
      m_cycleButtonPushed = true;
    }
    else if (digitalRead(m_cycleButtonPin) && m_cycleButtonPushed)
    {
      m_cycleButtonPushed = false;
    }
  }

  if (m_toggleButtonPin >= 0)
  {
    if (!digitalRead(m_toggleButtonPin) && !m_toggleButtonPushed)
    {
      Fade::stop();
      toggle();
      m_toggleButtonPushed = true;
    }
    else if (digitalRead(m_toggleButtonPin) && m_toggleButtonPushed)
    {
      m_toggleButtonPushed = false;
    }
  }
}

void FastLEDHubClass::setSpeed(uint8_t speed)
{
  m_speed = speed;
}

AnimationStatus FastLEDHubClass::getStatus()
{
  return m_status;
}

String FastLEDHubClass::getCurrentAnimationName()
{
  if (!m_currentAnimation)
    return "";

  return m_currentAnimation->getName();
}

void FastLEDHubClass::autostart()
{
  if (Config.startupAnimation == "")
    clear(true);
  else
    begin(getAnimation(Config.startupAnimation));

  m_autostartHandled = true;
}

void FastLEDHubClass::begin(Animation *animation)
{
  if (animation == NULL || (m_currentAnimation && m_currentAnimation->getName() == animation->getName()))
    return;

  clear(true);

  m_status = RUNNING;
  m_currentAnimation = animation;
  m_currentAnimation->reset();

  WebSocket::broadcastStatus();
  PRINTLN("Started '" + m_currentAnimation->getName() + "'");
}

void FastLEDHubClass::cycle()
{
  for (uint8_t i = 0; i < animations.size(); i++)
  {
    if (m_currentAnimation == getAnimation(i))
    {
      begin(getAnimation((i + 1) % animations.size()));
      return;
    }
  }
}

void FastLEDHubClass::stop()
{
  if (m_status == STOPPED)
    return;

  m_status = STOPPED;
  clear(true);
  m_currentAnimation = NULL;

  WebSocket::broadcastStatus();
  PRINTLN("Stopped animation");
}

void FastLEDHubClass::resume()
{
  if (m_status != PAUSED)
    return;

  m_status = RUNNING;

  WebSocket::broadcastStatus();
  PRINTLN("Resumed '" + m_currentAnimation->getName() + "'");
}

void FastLEDHubClass::restart()
{
  stop();
  begin(m_currentAnimation);

  PRINTLN("Restarted '" + m_currentAnimation->getName() + "'");
}

void FastLEDHubClass::pause()
{
  m_status = PAUSED;

  WebSocket::broadcastStatus();
  PRINTLN("Paused '" + m_currentAnimation->getName() + "'");
}

void FastLEDHubClass::toggle()
{
  if (m_status == RUNNING)
    pause();
  else if (m_status == STOPPED)
    begin(getAnimation(0));
  else
    resume();
}

void FastLEDHubClass::toggle(Animation *animation)
{
  if (m_currentAnimation && m_currentAnimation == animation && m_status == RUNNING)
    pause();
  else if (m_currentAnimation && m_currentAnimation == animation && m_status == PAUSED)
    resume();
  else
    begin(animation);
}

FastLEDHubClass FastLEDHub;
