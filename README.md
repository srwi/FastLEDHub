# FastLED Manager

Control multiple FastLED lightstrip animations on the ESP8266 without reuploading.

## Requirements

- Arduino libraries
  - ArduinoJson
  - LinkedList
  - Time
  - WebSockets
  - FastLED
- Custom libraries
  - [ESPEssentials](github.com/stnkl/ESPEssentials)
  - [WifiManager](https://github.com/tzapu/WiFiManager/tree/34d9a975b5339bb5510f5718fbd1530dff9de733) ('development' branch)

## How to use

- Create a new animation `ExampleAnimation` file in `/Animations/`
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
- Register animation in `LightstripWS2812B.ino`
    ```cpp
    registerAnimation(ExampleAnimation("Example animation name"));
    ```
- Upload sketch data folder `/data/`
- Upload sketch

## Notes

- There is no need to use `FastLED.show()`. It will be overwritten by a makro because FastLED Manager uses its own show function. This also means multiple calls within your loop function won't have an effect. `FastLED.show()` will be called right after your loop function has finished.
- There is currently no real support for `delay()` either. You can use a delay at the end of your loop function to slow down your animation but it won't block your code from continuing.
    ```
    void loop()
    {
      // do your animation stuff

      delay(100);
    }
    ```
  Also the *actual* delay time will be different depending on which speed is set within the web UI.