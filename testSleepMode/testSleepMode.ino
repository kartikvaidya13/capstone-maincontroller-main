#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include "user_interface.h"

extern os_timer_t *timer_list = nullptr; 

const char* ssid = "Registered4OSU";  // Register4OSU WiFi ssid 
// const char* password = "3NFuzSszH7sLzrEg";     // Registered4OSU WiFi password for unit 1
const char* password = "p96AUJfnUhVSVVQ4";     // Registered4OSU WiFi password for unit 2

// String url = "http://librarytracker-backend.herokuapp.com/update";  // server url for web application

void setup() {

  // for timer-based light sleep to work, the os timers need to be disconnected
  timer_list = nullptr; 
  Serial.begin(115200);  // sets baud rate for arduino
  delay(10);

  WiFi.begin(ssid, password);   // connects to Registered4OSU

  /* checks the WiFi status of the device until it is connected */
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
    Serial.print(".");
    delay(0);
  }
  Serial.println();

  // prints local IP
  Serial.print("Connected, IP address: ");;
  Serial.println(WiFi.localIP());
}

/* This function updates the counter of the webapp by sending a http request to the url */
void update() {
  // only sends an http request if the device is connected to the internet
  if (WiFi.status() == WL_CONNECTED) {

    WiFiClient client;
    HTTPClient http;
    Serial.println("Begin http connection");
    if (http.begin(client, "http://librarytracker-backend.herokuapp.com/update")) {
      // start connection and send HTTP header
      int httpCode = http.GET();
      if (httpCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpCode);
      }
      http.end();
    } else {
      Serial.println("Connection failure.");
    }
  }
}



void fpm_wakup_cb_func(void) {
  Serial.println("Light sleep is over, either because timeout or external interrupt");
  Serial.flush();
}



void loop() {
  sleep();
  delay(10000);
}

/* sets the device to sleep mode until input is detected */
void sleep() {
  // enable light sleep
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  
  wifi_fpm_set_wakeup_cb(fpm_wakup_cb_func);
  Serial.println("Entering sleep mode...");
  // sleep for 10 seconds
  long sleepTimeMilliSeconds = 10e3;
  // light sleep function requires microseconds
  wifi_fpm_do_sleep(sleepTimeMilliSeconds * 1000);

  // timed light sleep is only entered when the sleep command is
  // followed by a delay() that is at least 1ms longer than the sleep
  delay(sleepTimeMilliSeconds + 1);
}
