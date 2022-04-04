# ESP8266 Big Clock with WS2811 LED Strips

An ESP8266 clock utilising WS2811 LED Strips.

The Clock will operate in 12 or 24 Hour mode pulling the current time from NTP over WiFi.  The clock can also download the Sunrise and Sunset times from OpenWeatherMap if it is enabled and a valid API Key is entered.  In this mode the display is dimmed by user definable presets at Sunset and restored  at Sunrise.

The code takes Timezone into account.

The project consists of:

- Big_Clock.ino - All the code

- debug.h - A file containing macros to aid with debugging

- globals.h - A file containing global variables and definitions

- A case.  The case for this project was sourced from [Thigiverse](https://www.thingiverse.com) and 3d printed.  Case in black PLA and diffusers in translucent PLA

- An ESP8266

- WS2811 LED tape

You will also need a power supply capable of powering the LED's at 12v and a supply for the ESP8266.  Depending on your ESP8266 board, this may be 5v or 3.3v.  Two separate supplies can be used provided the ground's of both supplies are connected together.  Failure to do this will result in unreliable operation.

## Libraries utilised:

    [ArduinoJSON](https://arduinojson.org) (v6 or above) by Benoît Blanchon

    [FastLED](http://fastled.io) by Daniel Garcia et al

    ESP8266HTTPClient

    ESP8266WiFi

    Time.h



## Build

As mentioned above, the case was sourced from Thingiverse at this [link]([Retro 7 Segment Clock (Complete) by parallyze - Thingiverse](https://www.thingiverse.com/thing:3014572))

It is best to ensure that you have all the parts printed prior to assembly however if you are itching to get started, printing just the base frame is enough.  This case is intended for WS2812 LED's however some minor modifications to the LED's make it usable with WS2811.  Each Segment requires 2 strips of LED tape.  One strip is four sets of LED's long and the second is three sets long, each set is made up of 3 LED's, most tape is marked with cut lines.  These are soldered together with short pieces of insulated wire.  Starting at the left side of the centre segment, wrap the four segment tape around the segments moving counter clockwise, making sure they are fully down between the retaining tabs.  You should end up with 3 LED's per clock segment. The second piece of LED tape starts where the first finishes with the connecting wire between the segments and is wrapped counter clockwise with short connecting wires passing to the next digit. The first digit is where your wires will exit for connecting to power and ESP8266.

Each digit is constructed the same way.  Follow the build instructions on Thingiverse and refer to the image in this repository.

The only difference is the colon where you should end up with 2 LED's in one segment and three in the other.  This makes little difference to the overall brightness of this segment.  It is best to solder the strips as you go, pulling them away from the case a wee bit to facilitate the soldering.  This way you can ensure that the connecting wires are the correct length.  There is little room for bundling up wire.

## Code

All the code is in Big_Clock.ino with two includes.

### debug.h

This include contains a number of simple macros that are used to aid debugging.  These provide a more efficient way of using serial during debugging then disabling when the code is fully debugged.  To turn debugging off, comment out #DEBUG in globals.h

### globals.h

Contains all the global definitions required.  There are several which must be defined in order for the code to run.  They are:

**LAT/LON** - These should be set to your location in the world in order to use the Sunrise and Sunset capabilities for dimming the display.  These can be ignored if this functionality is not being used.  To disable this functionality comment out USE_DIM

**apikey** - If using the Sunrise/Sunset functionality, this must be a valid OneWeatherMap key.  You can get your own [OWM API key]([How to start to work with Openweather API - OpenWeatherMap](https://openweathermap.org/appid)) - it's free

**DATA_PIN** - Defines which GPIO pin FastLED will use for communication between the ESP8266 and the LED strips.  Set this to the pin you are using

**NUM_SEGMENTS** - Each WS2811 segment comprises of 3 LED's which in turn make up one segment of the clock.  A default clock which has four numbers and a colon in the centre has 30 segments.  This must be defined correctly for FastLED to know how many segments to control

**DEFAULT HUE/SATURATION/VALUE** - These are used to set the default colour of the clock segments.  These can be changed to suit your taste.

**Default colours** display are defined in CRGB space and can be changed.

**12/24** Hour operation depending on the define.

**number_matrix** - This array is used to determine which segments to turn on to represent each number of time.  1 means on, 0 means off.  If you wire your WS2811 segments differently, this will need to be modified to suit.

**ssid/password** - Change to suit the wireless network you are connecting to

**Time settings** for Timezone, NTP Sources Daylight Saving Offset and when the time is refreshed from NTP & OWM.

The remainder of the items in this file are either self explanatory or can be understood by reading the code.

## How does it work:

On startup Timezone is set and the LED's are initialised and a connection to WiFi is made.  The code will make multiple attempts to connect to WiFi as defined by WIFI_TRY_MAX, if this fails the LED's will flash a sequence to identify error.  Once WiFi connection is made, time is pulled via NTP in get_time() and stored in a number of variables.  EPOCH time is also stored and is used with Sunrise/Sunset.  An allowance is made for DST.  A timer interrupt is enabled which "ticks" each second and increments an internal epoch counter which is used in loop() to determine whether to increment minute and hour.  Once setup, the code in loop() simply checks the number of seconds, incrementing minutes and hours as necessary and updating the time on the display.

Code checks whether it is time for a refresh of time and whether the display should be dimmed depending on the settings for sunrise and sunset, if they are used.


