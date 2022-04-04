// Copyright 2022 Mark V. Marooth

// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
// associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and 
// to permit persons to whom the Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE 
// OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef _GLOBALS

     #define _GLOBALS

     // Setup serial output macros 
     #define DEBUG  

     #include "debug.h"

     #ifdef DEBUG
          // Default Serial Port Speed
          #define SERIAL_SPEED 115200
     #endif

     // Weather Stuff
     #define USE_DIM                                                      // Comment this out if you do not want the display to dim at Sunset
     const String LAT = "<Your Lat>";                                      // Latitude of where you live    
     const String LON = "<Your Lon>";                                       // Longitude of where you live
     const String apikey       = "<Your Key>";      // Your OpenWeatherMap API Key if you are using Sunrise/Sunset functionality
      
     //FastLED Stuff
     #include <FastLED.h>
     //Using GPIO Pin 5 for communication
     #define DATA_PIN 5
     #define NUM_SEGMENTS 30 
     #define DEFAULT_HUE 0
     #define DEFAULT_SATURATION 255
     #define DEFAULT_VALUE 127
     #define LED_BRIGHT 255                                                // Chance these to be as dim and as bright as you want (1-255)
     #define LED_DIM 20

     // These can be used to change HSV in the main code if it is required.
     uint8_t hue = DEFAULT_HUE;
     uint8_t saturation = DEFAULT_SATURATION;
     uint8_t value = DEFAULT_VALUE;

     // Default Colours if not using HSV
     const CRGB on_colour = CRGB::Red;
     const CRGB off_colour = CRGB::Black;
     const CRGB alert_colour = CRGB::White;
     
     // 12 or 24 Hour Clock display, valid values are HOUR12 and HOUR24
     // Defaults to 12 Hour if this is not defined
     #define HOUR24
     #define NUM_DIGITS 4

     // Array to hold which segments to turn on for a given number.
     // This matrix will depend on how the WS2811 segments are wired up
     // In my case, the centre segment is first in the line and then runs clockwise 
     // Through the remaining segments.  The number 0 should explain
     // This will need to be changed to suit other wiring schemes

     const int number_matrix [][7] = {{0,1,1,1,1,1,1},      //Zero
                                     {0,1,0,0,0,0,1},       //One
                                     {1,1,1,0,1,1,3},       //Two
                                     {1,1,1,0,0,1,1},       //Three
                                     {1,1,0,1,0,0,1},       //Four
                                     {1,0,1,1,0,1,1},       //Five
                                     {1,0,1,1,1,1,1},       //Six
                                     {0,1,1,0,0,0,1},       //Seven
                                     {1,1,1,1,1,1,1},       //Eight
                                     {1,1,1,1,0,1,1}};      //Nine
     //Segments for the colon
     #define COLON_1 14
     #define COLON_2 15
                                  
     // Wifi Credentials
     const char* ssid = "<Your SSID";
     const char* password = "<Your Password>";

     #define WIFI_TRY_MAX 10

     // Time Settings
     #define TIMEZONE "GMT0BST,M3.5.0/01,M10.5.0/02"             // Choose your time zone from: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv 
     #define TIME_SOURCE_PRIMARY "pool.ntp.org"                  // Change to suit your own requirements
     #define TIME_SOURCE_SECONDARY "time.nist.gov"               // Change to suit your own requirements
     #define DST_OFFSET 3600                                     // Change this to suit your own Daylight Saving Time
     #define REFRESH_HOUR 3                                      // The time when a NTP and OWM refresh is done, change to suit
     #define REFRESH_MINUTE 0

     const long utcOffsetInSeconds = 0;                          // Change/use this if you need it to meet your own timezone

     //Counters for Hours and Minutes
     byte hour, minute;

     // Flag to indicate whether an NTP & Weather refresh is needed and flags to be used
     byte trigger_refresh;
     #define REFRESH_NO 0
     #define REFRESH_PENDING 1
     #define REFRESH_DONE 2
     
     //Counter for epoch time, needs to be volatile as updated in the interrupt
     volatile unsigned long epoch_internal;
     volatile bool tick = false;
     
     // Sunrise and Sunset as epoch time
     unsigned long Sunrise;
     unsigned long Sunset;
#endif
