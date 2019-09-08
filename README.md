# FastLED Manager

Control multiple FastLED lightstrip animations on the ESP8266 without reuploading and more...

![FastLEDManager web app screenshot](screenshots/screenshot.png "FastLED Manager web app")

## Features

- Start/stop/pause multiple animations
- Set custom static color
- Set animation speed
- Control saturation
- Alarm: Be woken up to an animation slowly fading in
- Sunset: Automatically fade in an animation when the sun sets at your location
- Spectroscope: Display spectroscope data from your computer's sound card
- Compatible with [ha-bridge](https://github.com/bwssytems/ha-bridge) via [http requests](https://github.com/stnkl/FastLEDManager/blob/master/Webserver.cpp)

## Requirements

- Arduino libraries
  - ArduinoJson
  - LinkedList
  - WebSockets
  - FastLED
- Custom libraries
  - [ESPEssentials](github.com/stnkl/ESPEssentials)
  - [WifiManager](https://github.com/tzapu/WiFiManager/tree/34d9a975b5339bb5510f5718fbd1530dff9de733) ('development' branch)

## Compile

1. Download FastLED Manager from the [releases](https://github.com/stnkl/FastLEDManager/releases) page.
2. Rename the folder containing the `FastLEDManager.ino` to `FastLEDManager`
3. Make sure you have installed all the above libraries (Install Arduino libraries with the Arduino IDE. Download custom libraries from github and extract them into your Arduino/libraries folder)
4. Select your board and flash size (we need about 50kb of SPIFFS)
5. Upload sketch
6. Upload SPIFFS data using the [data upload plugin](https://github.com/esp8266/arduino-esp8266fs-plugin)

## Add a new animation

- Create a new animation `ExampleAnimation.h` file in `/Animations/`
    ```cpp
    #pragma once

    class ExampleAnimation : public Animation
    {
      public:
        using Animation::Animation;

        // other functions and variables if needed

        void reset() {
          // your standard reset function
        }

        void loop()
        {
          // your standard loop function
        }
    };
    ```
- Include and register animation in `LightstripWS2812B.ino`
    ```cpp
    #include "Animations/ExampleAnimation.h"

    ...

    registerAnimation(new ExampleAnimation("Example animation name"));
    ```