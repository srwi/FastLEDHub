# FastLEDHub

![ESP8266](https://img.shields.io/badge/ESP-8266-000000.svg?colorB=blue)
![ESP32](https://img.shields.io/badge/ESP-32-000000.svg?colorB=blue)
[![arduino-library-badge](https://www.ardu-badge.com/badge/FastLEDHub.svg?)](https://www.ardu-badge.com/FastLEDHub)
[![LGPL-2.1 license](https://img.shields.io/github/license/srwi/FastLEDHub)](https://github.com/srwi/FastLEDHub/blob/master/LICENSE)

FastLEDHub allows you to manage all of your [FastLED]([FastLED](https://github.com/FastLED/FastLED)) sketches on the ESP8266 and ESP32 with minimal changes to your existing code. It requires little knowledge about the ESP8266/ESP32 platform making it an ideal playground for beginners getting started with FastLED animations.

## Features

- Control multiple animations via an intuitive web interface
- Adjust brightness and animation speed globally
- Add widgets like color pickers or sliders to control complex animations
- Alarm: Be woken up to an animation slowly fading in
- Sunset: Automatically fade in an animation when the sun sets at your location
- Control animations and brightness using hardware inputs
- Incorporate audio spectrum data into your animations
- Control animations using HTTP requests for easy automation

## Demo

![FastLEDHub web app screenshot](https://user-images.githubusercontent.com/17520641/163568239-527e2239-536c-4324-8e0f-5c712b3d4635.gif)


## Installation

### Library dependencies

- ArduinoJson ≥ 7.0
- ESPEssentials ≥ 2.1
- FastLED
- LinkedList
- WebSockets
- WiFiManager ≥ 2.0

### Official releases via the Arduino IDE v1.8+
1. Open the Arduino IDE
2. Navigate to _"Sketch"_ &#8594; _"Include Library"_ &#8594; _"Manage Libraries..."_
3. Search for `FastLEDHub` and install the desired version

### Manual Installation

1. Make sure you have installed the dependencies above
2. Download the desired version from the [releases](https://github.com/srwi/FastLEDHub/releases) page
3. Extract the contents of the downloaded zip file
4. Rename the extracted folder to `FastLEDHub`
5. Move this folder to your libraries directory `~/Arduino/libraries`)
6. Restart your Arduino IDE

### Using Git

```bash
cd ~/Arduino/libraries
git clone https://github.com/srwi/FastLEDHub.git
```

To update to the latest version of the library

```bash
cd ~/Arduino/libraries/FastLEDHub && git pull
```

## Usage

Using FastLEDHub to manage your FastLED animations requires mainly four steps:

- Creating the main sketch to initialize your lightstrip with FastLEDHub
- Creating an animation or modifying an existing sketch to be compatible with FastLEDHub
- Registering your animations in the main sketch
- Uploading the web interface files to the flash storage

### Creating the main sketch

```cpp
#include <FastLEDHub.h>

#define NUM_LEDS 100
#define LED_TYPE WS2812B
#define LIGHTSTRIP_PIN 5

CRGB leds[NUM_LEDS];

void setup()
{
  FastLEDHub.initialize("Project Name");
  FastLEDHub.addLeds<LED_TYPE, LIGHTSTRIP_PIN, GRB>(leds, NUM_LEDS);
}

void loop()
{
  FastLEDHub.handle();
}
```

Change `NUM_LEDS`, `LED_TYPE` and `LIGHTSTRIP_PIN` according to your hardware configuration. You may notice that this is not different than setting up a regular FastLED sketch apart from using `FastLEDHub` instead of `FastLED`.

*Note:* By default FastLEDHub will apply gamma correction to the brightness value. To disable this behavior call `FastLEDHub.initialize("Project Name", false)` instead.

### Adding a new animation

Create a new animation file `Animations/ExampleAnimation.h`:

```cpp
#pragma once

#include <FastLEDHub.h>

extern CRGB leds[];

class ExampleAnimation : public Animation
{
public:
    using Animation::Animation;

    // add variables and other functions if needed

    void reset()
    {
      // set initial state variables
    }

    void loop()
    {
      // animate leds
    }
};
```

While creating your animation proceed as you usually would with FastLED by defining the `reset` and `loop` functions. `reset` will be called each time an animation gets started. Use this function to reset the state variables of your animation to its starting values. It will not be called when resuming the animation from the paused status. `loop` will be called repeatedly as long as the animation is running.

Keep in mind the following important differences to just using FastLED:
- The regular `setup` function is called `reset` to emphasize its purpose.
- Since `leds` has already been defined in the main sketch, simply indicate its existence with `extern CRGB leds[]`.
- Within your animation use `FastLEDHub[0].size()` instead of `NUM_LEDS` to get the number of leds. If you are using multiple lightstrips change the index accordingly.
- Every time you may want to use `FastLED` use `FastLEDHub` instead. Since `FastLEDHub` inherits from `FastLED` all member functions will be available just like before. FastLEDHub just adds some stuff on top of that.

If you want to convert an existing FastLED sketch (e.g. from [atuline/FastLED-Demos](https://github.com/atuline/FastLED-Demos)), so it can be handled by FastLEDHub, generally those are the necessary changes you have to perform. Have a look at the examples for further insights.

### Registering animations

In your main sketch include your animations and register them at the end of the `setup` function:

```cpp
#include "Animations/ExampleAnimation.h"

...

FastLEDHub.registerAnimation(new ExampleAnimation("Example animation name"));
```

The animation name can be any unique string and will be used to identify animations in the web interface.

### Uploading web interface files

To be able to access the web interface, several files have to be uploaded to the flash storage. They are located within the `data` folder inside FastLEDHub's library folder. To upload those files there are two easy options:

- Copy the whole `data` folder into your sketch directory and upload the files using the Arduino filesystem uploader ([ESP8266](https://github.com/esp8266/arduino-esp8266fs-plugin), [ESP32](https://github.com/me-no-dev/arduino-esp32fs-plugin)).
- After the sketch has been flashed to the device upload each file within the `data` folder individually by accessing `http://<device-ip>/edit`.

## Additional features

### Custom color pickers

FastLEDHub allows you to register multiple color pickers to use as parameters for your animations. This allows you to integrate custom colors, gradients and more.

To add a color picker use

```cpp
FastLEDHub.registerColorPicker(new ColorPicker("Primary Color", CRGB(255, 0, 0)));
FastLEDHub.registerColorPicker(new ColorPicker("Secondary Color", CRGB(0, 255, 0), "paint-bucket"));
```

Within the web interface FastLEDHub uses [Bootstrap icons](https://icons.getbootstrap.com/#icons) to allow you to further differentiate between color pickers. Here `paint-bucket` refers to the respective icon class.

To access those colors within your animation use

```cpp
CRGB primaryColor = FastLEDHub.getColorPicker("Primary Color")->value; // access by name
CRGB secondaryColor = FastLEDHub.getColorPicker(1)->value;             // access by index
```

### Pre-defined and custom sliders

You can add custom numeric sliders of type `int16_t` to adjust variables of animations dynamically. FastLEDHub automatically adds two sliders for brightness (0-255, default: 127) and animation speed (0-255, default: 127). Both of these fixed sliders have been integrated tightly into FastLEDHub and don't require any further attention. By default changing the brightness will apply gamma correction automatically. Adjusting the speed will affect the effective delay of `FastLEDHub.delay()` to speed up or slow down animations. To prevent this explicitly use `FastLED.delay()`.

To add more custom sliders simply register them in the main sketch via

```cpp
FastLEDHub.registerSlider(new Slider("Hue", 0, 359, 180, 1));
FastLEDHub.registerSlider(new Slider("Saturation", 150, 255, 200, 1, "palette"));
```

Again `"palette"` refers to an optional [Bootstrap icon](https://icons.getbootstrap.com/#icons) and the slider name can be any unique string identifying the slider in the web interface.

To access custom slider values inside of your animation use

```cpp
int16_t hue = FastLEDHub.getSlider(2)->value;                   // access by index
int16_t saturation = FastLEDHub.getSlider("Saturation")->value; // access by name
```

*Remember:* Since FastLEDHub comes with two pre-defined sliders `Brightness` and `Speed` the first custom slider will have index `2`.

### Hardware inputs

FastLEDHub supports a potentiometer for brightness adjustments and push buttons to play/pause and cycle through animations. They have to be specifically enabled with

```cpp
FastLEDHub.enablePotentiometer(potentiometerPin);
FastLEDHub.enableToggleButton(togglePin);
FastLEDHub.enableCycleButton(cyclePin);
```

### Alarm and sunset

Setting up an alarm in the web interface will fade in a user defined animation over any period of time to wake you up in the morning. You can optionally set a different animation to be started after the fade in period has ended (i.e. full brightness has been reached).

Similarly the sunset feature will fade in an animation as soon as the sun sets at your location. Please configure latitude, longitude and time zone in the web interface beforehand.

### Audio spectrum data

[FastLEDHub-AudioViz](https://github.com/srwi/FastLEDHub_AudioViz) allows you to send audio spectrum data from a Windows audio device to your ESP32/ESP8266. The transmitted data consists of 16 bins corresponding to different frequency ranges in the audio spectrum. Data is transmitted via a websocket connection and can be used by accessing the `FastLEDHub.spectrumData` array from within your animation:

```cpp
uint8_t lowFrequencies = FastLEDHub.spectrumData[0];
uint8_t highFrequencies = FastLEDHub.spectrumData[SPECTRUM_LENGTH - 1];
```

`SPECTRUM_LENGTH` defines the number of bins (16). The `Spectrogram` example shows one way to use the spectrum data in your animations.

### Control via HTTP requests

Most functions can be triggered via HTTP requests:

- Begin animation by name: `http://<device-ip>/begin?animation=<animation-name>`
- Begin animation by index: `http://<device-ip>/begin?index=<animation-index>`
- Stop animation: `http://<device-ip>/stop`
- Pause animation: `http://<device-ip>/pause`
- Resume animation: `http://<device-ip>/resume`
- Toggle animation: `http://<device-ip>/toggle`
- Restart animation: `http://<device-ip>/restart`
- Trigger sunset: `http://<device-ip>/sunset`
- Trigger alarm: `http://<device-ip>/alarm`
- Reset device: `http://<device-ip>/reboot`

## License & Attribution

FastLEDHub is licensed under LGPL-2.1 and uses the [sunrise-sunset.org](https://sunrise-sunset.org/api) api to retrieve sunset times.
