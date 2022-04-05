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


#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <FastLED.h>
#include "globals.h"
#include <time.h>
#include <ESP8266WiFi.h>

time_t currentTime;
tm tm;

// Define the array of leds
CRGB leds[NUM_SEGMENTS];

//##################################################
//               WiFi Functions
//##################################################

bool wifi_init(void){
     byte wifi_connect_count = 0;
     WiFi.begin(ssid, password);
     while((WiFi.status() != WL_CONNECTED) && wifi_connect_count < WIFI_TRY_MAX){
          delay(1000);
          wifi_connect_count++;
     }
     if(WiFi.status() != WL_CONNECTED)
          return false;
     else
          return true;
}

void wifi_on(void){
    if(wifi_init() == false){
          dout_s("\nWiFi failed to initilise.");       
          flash_digits(5, alert_colour);
          flash_segment(2, 5, on_colour);
     }
     else{
          dout_s("\n Wifi Initialised");
     }
}
void wifi_off(void) {
     WiFi.disconnect();
     WiFi.mode(WIFI_OFF);
     dout_s("\nWifi Off");
}

//##################################################
//            Time Functions
//##################################################

bool get_time(byte& hours, byte& minutes, volatile unsigned long& epoch_internal){
    
     if(!time(&currentTime)){
          flash_digits(5, alert_colour);
          flash_segment(18, 5, on_colour);
     }
     
     localtime_r(&currentTime, &tm);
     
     minutes = tm.tm_min;
     hours = tm.tm_hour;
     epoch_internal = currentTime;

     //Cater for DST
     if(tm.tm_isdst == 1)
          epoch_internal = epoch_internal + DST_OFFSET;

     dout_v("\nHour: ", hours);
     dout_v("\nMinute: ", minutes);
     dout_v("\nEpoch: ", epoch_internal);

     if(epoch_internal < 10000){         // If we end up with a screwy epoch then time is not correct
          return false;
     }
     else{
          return true;
     }
}

void refresh_time(void){
     FastLED.clear();
     FastLED.show();
     wifi_on();
     get_time(hour, minute, epoch_internal);
     #ifdef USE_DIM
          get_weather();
     #endif
     wifi_off();
     display_time(hour, minute);
     timer_off();
     timer_on();
}


//##################################################
//               Display Functions
//##################################################

void initialise_display(void){

     // Set the initial LED Brightness
     FastLED.setBrightness(LED_BRIGHT);
     
     // Clear the entire display
     FastLED.clear();
     FastLED.show();
     for(int x = 0; x < NUM_SEGMENTS; x++){
          leds[x] = on_colour;
          FastLED.show();
          delay(200);         // A reasonable use of delay
     }
     FastLED.clear();
     FastLED.show();
}

void display_number(int position, int display_num){
     int start_position = (position - 1) * 7;

     if(position > 2) 
          start_position = start_position + 2;  //Cater for the two segments that flash the seconds
     
     for(byte x = 0; x < 7; x++){
          leds[x + start_position] = off_colour;
          if(number_matrix[display_num][x] == 1){
               // leds[x + start_position] = CHSV(hue, saturation, value);
               leds[x + start_position] = on_colour;

          }
     }
     FastLED.show();
}

void display_time(byte hours,byte minutes){
    
     // Not sure whether to update the entire time or just what has changed
     // Will see when this code actually runs on the clock
     FastLED.clear();
     FastLED.show();

     if(hours < 10){
          #ifdef HOUR24
               display_number(1, 0);
          #endif
          display_number(2, hours);
     }
     if(hours >=10 && hours <= 12){
          #ifdef HOUR24
               display_number(1,1);
          #endif
          display_number(2, int(hours%10));
     }
     if(hours > 12){
          #ifdef HOUR24
               display_number(1, int(hours/10));
               display_number(2, (hours%10));
          #else
               display_number(2, (hours%12));
          #endif
     }

     if(minutes < 10){
          display_number(3, 0);
     }
     else{
          display_number(3, int(minutes/10));
     }
     display_number(4,(minutes%10));
}

//##################################################
//         Warning/Status Functions
//##################################################
void flash_digits(byte repeat, CRGB flash_colour){
     FastLED.clear();
     FastLED.show();
     for(byte r = 0; r < repeat; r++){
          for(byte x = 0; x < NUM_SEGMENTS; x++){
               leds[x] = flash_colour;
          }
          FastLED.show();
          delay(500);
          FastLED.clear();
          FastLED.show();
          delay(500);
     }
     FastLED.clear();
     FastLED.show();
}

void flash_segment(byte segment, byte repeat, CRGB flash_colour){
     FastLED.clear();
     FastLED.show();
     for(byte r = 0; r<repeat;r++){
          leds[segment] = flash_colour;
          FastLED.show();
          delay(500);
          leds[segment] = off_colour;
          FastLED.show();
          delay(500);
          
     }
}

void colon(bool display_mode){
     if(display_mode == true) {
          leds[COLON_1] = on_colour;
          leds[COLON_2] = leds[COLON_1];
     }
     else{
          leds[COLON_1] = off_colour;
          leds[COLON_2] = off_colour;
     }
     FastLED.show();
}

//##################################################
//               Weather Functions
//##################################################

bool get_weather(void){
     WiFiClient client;
     HTTPClient http;
     
     String uri = "/data/2.5/weather?lat=" + LAT +"&lon=" + LON + "&appid=" + apikey;

      http.begin(client, "api.openweathermap.org", 80, uri); 
      
     int httpCode = http.GET();
     if (httpCode == HTTP_CODE_OK) {
          DynamicJsonDocument doc(1024);
          DeserializationError error = deserializeJson(doc, http.getStream());
          if (error) {                                             // Test if parsing succeeds.
               dout_v("\ndeserializeJson() failed: ", error.c_str());
               return false;
          }
          JsonObject root = doc.as<JsonObject>();
          Sunrise = root["sys"]["sunrise"].as<long>();
          Sunset = root["sys"]["sunset"].as<long>();
          dout_s("\nget_weather");
          dout_v("\nSunrise: ", Sunrise);
          dout_v("\nSunset: ", Sunset);
          dout_v("\nEPOCH Internal: ", epoch_internal);
          client.stop();
          http.end();
          return true;
     }
     else
          return false;
}

//##################################################
//               Interrupt Functions
//##################################################
void timer_off(void){
     noInterrupts();
}

void timer_on(void){
     timer0_isr_init();
     timer0_attachInterrupt(ISR);
     timer0_write(ESP.getCycleCount() + 80000000L);
     interrupts();   
}

// Timer Interrupt to incrment epoch
ICACHE_RAM_ATTR void ISR(void){
     timer0_write(ESP.getCycleCount() + 80000000L);
     epoch_internal++;
     tick = true;
}

//##################################################
//               Setup
//##################################################
void setup() { 

     //Set NTP and Timezone
    configTime(0, 0, TIME_SOURCE_PRIMARY, TIME_SOURCE_SECONDARY);
    setenv("TZ", TIMEZONE, 1);

     Sunrise = 0;
     Sunset = 0;
     trigger_refresh = REFRESH_NO;
     
     #ifdef DEBUG
          Serial.begin(115200);
          while (!Serial);
          dout_s("\n Serial Started");
     #endif
   
     //Using WS2811 LED's
     FastLED.addLeds<WS2811, DATA_PIN, GRB>(leds, NUM_SEGMENTS);

     initialise_display();
     
     wifi_on();

     get_time(hour, minute, epoch_internal);
     
    #ifdef USE_DIM
          if(!get_weather()){
               dout_s("\nFailed to get Sunrise & Sunset");
               flash_digits(5, alert_colour);
               flash_segment(9, 5, on_colour);
          }
     #endif
     
     // As we have grabbed the time from NTP and Sunrise/Sunset from OWM, no longer need WiFi so turn it off
     wifi_off();
     
     display_time(hour, minute);

     //Interrupts on
     timer_off();
     timer_on();
}

void loop() { 
     
     bool change_display = false;

     if(trigger_refresh == REFRESH_PENDING){
          dout_s("\n  Refresh Triggered");
          refresh_time();
          trigger_refresh = REFRESH_DONE;
     }

     // Flash the colon every second, not done in ISR on the basis that the very minimum should be done 
     // in an Interrupt, easily handled here.
     
     if(epoch_internal%2 == 0)
          colon(true);
     else
          colon(false);

     if(tick){
          tick = false;
          if(epoch_internal % 60 == 0){
               change_display = true;
               minute++;
               if(minute > 59){
                    minute = 0;
                    hour++;
                    if(hour > 23){
                         hour = 0;
                    }  //Hour Past Midnight
               }  //Increment Hour
          }  //Epoch mod 60 means that minute has changed

          //If we are at the refresh point, set the time refresh to pending
          if(hour == REFRESH_HOUR && minute == REFRESH_MINUTE && trigger_refresh == REFRESH_NO){
               trigger_refresh = REFRESH_PENDING;
          }

          //Now test that we have gone at least one minute past the refresh time and reset the trigger
          if(hour == REFRESH_HOUR && minute >= REFRESH_MINUTE + 1){
               trigger_refresh = REFRESH_NO;
          }
     }  //Second has rolled over from Interrupt
    
     if(change_display){
          
          #ifdef USE_DIM
               if(epoch_internal > Sunset && Sunset != 0){
                    dout_s("\nSunset");
                    dout_v("\nInternal epoch: ", epoch_internal);
                    dout_v("\nSunset: ", Sunset);
                    dout_s("\n Dim the LEDS");
                    FastLED.setBrightness(LED_DIM);
                    Sunset = 0;         // Do this so that Sunset does not trigger a brightness until the next read from OWM
                    Sunrise = 0;
               }
               if(epoch_internal > Sunrise && Sunrise != 0){
                    dout_s("\nSunrise");
                    dout_v("\nInternal epoch: ", epoch_internal);
                    dout_v("\nSunrise: ", Sunrise);
                    Sunrise = 0;        // Do this so that Sunrise does not trigger a brightness change until the next read from OWM
                    dout_s("\n Brighten the LEDs");
                    FastLED.setBrightness(LED_BRIGHT);
               }
          #endif
          
          display_time(hour, minute);
          change_display = false;
     }
 }
