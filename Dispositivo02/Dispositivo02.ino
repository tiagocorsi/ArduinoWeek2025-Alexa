/*
 Name:		  Dispositivo02.ino
 Created:	  25/03/2025
 Author:	  Tiago H. Corsi
 Comments:  Exemplo criado para a palestra Arduino Week 2025
*/

// Uncomment the following line to enable serial debug output
#define ENABLE_DEBUG

#ifdef ENABLE_DEBUG
  #define DEBUG_ESP_PORT Serial
  #define NODEBUG_WEBSOCKETS
  #define NDEBUG
#endif 

#include <Arduino.h>
#if defined(ESP8266)
  #include <ESP8266WiFi.h>
#elif defined(ESP32) || defined(ARDUINO_ARCH_RP2040)
  #include <WiFi.h>
#endif

#include "credentials.h"
#include "SinricPro.h"
#include "SinricProTemperaturesensor.h"
#include "DHT.h"


#define BAUD_RATE         115200              // Change baudrate to your need (used for serial monitor)
#define EVENT_WAIT_TIME   60000               // send event every 60 seconds

#if defined(ESP8266)
  #define DHT_PIN    2
#elif defined(ESP32)
  #define DHT_PIN    5
#endif

#define DHTTYPE DHT11   // DHT 11

float temperature;                            // actual temperature
float humidity;                               // actual humidity
float lastTemperature;                        // last known temperature (for compare)
float lastHumidity;                           // last known humidity (for compare)

DHT dht(DHT_PIN, DHTTYPE);

void handleTemperaturesensor() {
  if (SinricPro.isConnected() == false) {
    Serial.printf("Not connected to Sinric Pro...!\r\n");
    return; 
  }

  static unsigned long last_millis;
  unsigned long        current_millis = millis();
  if (last_millis && current_millis - last_millis < EVENT_WAIT_TIME) return;
  last_millis = current_millis;
  
  temperature = dht.readTemperature();          // get actual temperature in °C
//  temperature = dht.readTemperature(true);    // get actual temperature in °F
  humidity = dht.readHumidity();                // get actual humidity

  if (isnan(temperature) || isnan(humidity)) { // reading failed... 
    Serial.printf("DHT reading failed!\r\n");  // print error message
    return;                                    // try again next time
  } 

  Serial.printf("Temperature: %2.1f Celsius\tHumidity: %2.1f%%\r\n", temperature, humidity);

  if (temperature == lastTemperature && humidity == lastHumidity) {
    Serial.printf("Temperature did not changed. do nothing...!\r\n");
    return; 
  }

  SinricProTemperaturesensor &mySensor = SinricPro[TEMP_SENSOR_ID];  // get temperaturesensor device
  bool success = mySensor.sendTemperatureEvent(temperature, humidity); // send event
  if (success) {  
    Serial.printf("Sent!\r\n");
  } else {
    Serial.printf("Something went wrong...could not send Event to server!\r\n"); // Enable ENABLE_DEBUG to see why
  }

  lastTemperature = temperature;  // save actual temperature for next compare
  lastHumidity = humidity;        // save actual humidity for next compare 
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

  #if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP); 
    WiFi.setAutoReconnect(true);
  #elif defined(ESP32)
    WiFi.setSleep(false); 
    WiFi.setAutoReconnect(true);
  #endif

  WiFi.begin(WIFI_SSID, WIFI_PASS); 

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  IPAddress localIP = WiFi.localIP();
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %d.%d.%d.%d\r\n", localIP[0], localIP[1], localIP[2], localIP[3]);
}

// setup function for SinricPro
void setupSinricPro() {
  // add device to SinricPro
  SinricProTemperaturesensor &mySensor = SinricPro[TEMP_SENSOR_ID];
    
  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  //SinricPro.restoreDeviceStates(true); // Uncomment to restore the last known state from the server.
  SinricPro.begin(APP_KEY, APP_SECRET);  
}

// main setup function
void setup() {
  Serial.begin(BAUD_RATE); 
  Serial.printf("\r\n\r\n");
  dht.begin();

  setupWiFi();
  setupSinricPro();
}

void loop() {
  SinricPro.handle();
  handleTemperaturesensor();
}
