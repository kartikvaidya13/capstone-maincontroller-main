#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include "user_interface.h"


const char* ssid = "Registered4OSU";  // Register4OSU WiFi ssid 
const char* password1 = "p96AUJfnUhVSVVQ4";     // Registered4OSU WiFi password for unit 1
const char* password2 = "CmR3MGtnjwGNTPyz";     // Registered4OSU WiFi password for unit 2
const char* password3 = "f9e5HTx49UHr7bdv";     // Registered4OSU WiFi password for unit 3
const char* password4 = "NxYuHSn6UfcuH9wz";     // Registered4OSU WiFi password for unit 4
const char* password5 = "sYqr6gy4AiA9KNp6";     // Registered4OSU WiFi password for unit 5

const String macAddress1 = "E8:DB:84:95:D3:78";   // macAddress for unit 1
const String macAddress2 = "E8:DB:84:95:B2:B1";   // macAddress for unit 1
const String macAddress3 = "BC:FF:4D:48:A0:B3";   // macAddress for unit 1
const String macAddress4 = "E8:DB:84:96:9F:BA";   // macAddress for unit 1
const String macAddress5 = "BC:FF:4D:47:DC:95";   // macAddress for unit 1

bool isCounted = false;
bool exiting = false;
bool entering = false;

void setup() {
  Serial.begin(115200);  // sets baud rate for arduino
  delay(1000);

  /* Pin set up */ // read at low
  pinMode(3,INPUT); 
  pinMode(4,INPUT); 
  pinMode(5,INPUT); 
  pinMode(9,INPUT); 
  pinMode(12,INPUT); 
  pinMode(14,INPUT); 

  /* determines which password to use */
  String password = "";
  String macAddress = WiFi.macAddress();

  if(macAddress.compareTo(macAddress1) == 0) {
    password = password1; 
  }
  else if (macAddress.compareTo(macAddress2) == 0) {
    password = password2; 
  }
  else if (macAddress.compareTo(macAddress3) == 0) {
    password = password3; 
  }
  else if (macAddress.compareTo(macAddress4) == 0) {
    password = password4; 
  }
  else if (macAddress.compareTo(macAddress5) == 0) {
    password = password5; 
  }
  else {
    Serial.println("Error: MAC address not found");
  }
  
  setupWIFI(ssid,password);
}


/*  Connects ESP8266 to local WiFi */
void setupWIFI(String ssid, String password){
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
void update(bool countUp) {

  // determine url to http request to 
  String url = "";
  String urlCountUp = "http://librarytracker-backend.herokuapp.com/696e637265617365";
  String urlCountDown = "http://librarytracker-backend.herokuapp.com/6465637265617365";

  //TODO write code to determine count up or down
  if(countUp) {
    url = urlCountUp;
  }
  else {
    url = urlCountDown; 
  }

  // only sends an http request if the device is connected to the internet
  if (WiFi.status() == WL_CONNECTED) {

    WiFiClient client;
    HTTPClient http;
    Serial.println("Begin http connection");
    if (http.begin(client, url)) {
      // start connection and send HTTP header
      int httpCode = http.GET();
      if (httpCode > 0) {
         // return HTTP response code
        Serial.print("HTTP Response code: ");
        Serial.println(httpCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        // return error code
        Serial.print("Error code: ");
        Serial.println(httpCode);
      }
      http.end();
    } else {
      // prints if HTTP get request fails 
      Serial.println("Connection failure.");
    }
  }
}

/* determines if top sensors have been tripped */
bool topSensors() {
   return ((digitalRead(5) == 0)); 
}

/* determines if bottom sensors have been tripped */
bool botSensors() {
   return ((digitalRead(12) == 0)); 
}

void loop() { 
  bool countUp = false; 


  /* determines which sensors have been tripped */
  if(topSensors()) {
    countUp = true;
    Serial.println("Enter detected");
    update(countUp);
    while(topSensors()) {
      Serial.print(".");
      delay(100);   
    }
    delay(400);
  }
  if(botSensors()) {
    countUp = false;
    Serial.println("Exit");
    update(countUp);
    while(botSensors()) {
      Serial.print(".");
      delay(100);   
    }
    delay(400);
  }
 yield();
}
