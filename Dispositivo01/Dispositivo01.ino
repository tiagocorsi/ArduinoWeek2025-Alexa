/*
 Name:		  Dispositivo01.ino
 Created:	  25/03/2025
 Author:	  Tiago H. Corsi
 Comments:  Exemplo criado para a palestra Arduino Week 2025
*/

#define DEBUG_FAUXMO

#include <Arduino.h>

#ifdef ESP32
    #include <WiFi.h>
#else
    #include <ESP8266WiFi.h>
#endif

#include "fauxmoESP.h"
#include "credentials.h"

// -----------------------------------------------------------------------------

fauxmoESP fauxmo;

// -----------------------------------------------------------------------------

#define SERIAL_BAUDRATE   115200

#define LED_SALA          0
#define LED_QUARTO        2

#define ID_SALA           "Lâmpada Sala"
#define ID_QUARTO         "Lâmpada Quarto"

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

    WiFi.mode(WIFI_STA);
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

// -----------------------------------------------------------------------------

void setup() {
    
    //Serial
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println();
    Serial.println();

    // LEDs
    pinMode(LED_SALA, OUTPUT);
    pinMode(LED_QUARTO, OUTPUT);

    digitalWrite(LED_SALA, HIGH);
    digitalWrite(LED_QUARTO, HIGH);

    // Wifi
    wifiSetup();
    
    //fauxmoESP
    fauxmo.createServer(true);
    fauxmo.setPort(80);
    fauxmo.enable(true);

    // adiciona dispositivos virtuais
    fauxmo.addDevice(ID_SALA);
    fauxmo.addDevice(ID_QUARTO);

    // Função Callback quando é recebido um comando da Alexa.
    fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state, unsigned char value) {
             
        Serial.printf("[MAIN] Device #%d (%s) state: %s value: %d\n", device_id, device_name, state ? "ON" : "OFF", value);
        if (strcmp(device_name, ID_SALA)==0) {
            digitalWrite(LED_SALA, state ? LOW : HIGH);
        } else if (strcmp(device_name, ID_QUARTO)==0) {
            digitalWrite(LED_QUARTO, state ? LOW : HIGH);
        }
    });
}

// -----------------------------------------------------------------------------

void loop() {

    fauxmo.handle();

    // This is a sample code to output free heap every 5 seconds
    // This is a cheap way to detect memory leaks
    static unsigned long last = millis();
    if (millis() - last > 5000) {
        last = millis();
        Serial.printf("[MAIN] Free heap: %d bytes\n", ESP.getFreeHeap());
    }
}
