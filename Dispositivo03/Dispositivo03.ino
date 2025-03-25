/*
 Name:		  Dispositivo03.ino
 Created:	  25/03/2025
 Author:	  Tiago H. Corsi
 Comments:  Exemplo criado para a palestra Arduino Week 2025
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define TOPICO_SUB_LED_VERDE "luz/led_verde/switch"  //comando liga/desliga
#define TOPICO_PUB_LED_VERDE "luz/led_verde/state"  //confirma comando

#define TOPICO_SUB_LED_VERMELHO "luz/led_vermelho/switch"  //comando liga/desliga
#define TOPICO_PUB_LED_VERMELHO "luz/led_vermelho/state"  //confirma comando

#define TOPICO_SUB_LED_AMARELO "luz/led_amarelo/switch"  //comando liga/desliga
#define TOPICO_PUB_LED_AMARELO "luz/led_amarelo/state"  //confirma comando

#define TOPICO_PUB_TEMPERATURA_DHT11   "temperatura/escritorio/state" //leitura valor temperatura

#define ID_MQTT  "ESP32-DISPOSITIVO_03"    

#define DHT_PIN    5
#define DHTTYPE DHT11
#define EVENT_WAIT_TIME   10000 // 10 segundos

#define LED_VERDE 4
#define LED_VERMELHO 27
#define LED_AMARELO 25

float temperature;
float humidity;
float lastTemperature;
float lastHumidity;

char temperature_string[5];
char humidity_string[5];
  
DHT dht(DHT_PIN, DHTTYPE);
 
WiFiClient espClient;
PubSubClient MQTT(espClient);

void init_leds();
void init_serial();
void init_wifi();
void init_mqtt();
void reconnect_wifi(); 
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void verify_connections();
void handleTemperaturesensor();

void setup() 
{
    init_leds();
    init_serial();
    init_wifi();
    init_mqtt();
}

void init_leds() 
{
    // LEDs
    pinMode(LED_VERDE, OUTPUT);
    digitalWrite(LED_VERDE, LOW);

    pinMode(LED_VERMELHO, OUTPUT);
    digitalWrite(LED_VERMELHO, LOW);

    pinMode(LED_AMARELO, OUTPUT);
    digitalWrite(LED_AMARELO, LOW);
}
  
void init_serial() 
{
    Serial.begin(115200);
}
 
void init_wifi() 
{
    delay(10);
    Serial.println("------Conexao WI-FI------");
    Serial.print("Conectando-se na rede: ");
    Serial.println(SSID);
    Serial.println("Aguarde");
    reconnect_wifi();
}
  
void init_mqtt() 
{
    MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
    MQTT.setCallback(mqtt_callback);
}
  
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
 
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }

    if (strcmp(topic, TOPICO_SUB_LED_VERDE) == 0) {
      if (msg == "ON") {
        Serial.println("Led Verde - ON");
        digitalWrite(LED_VERDE, HIGH);
        MQTT.publish(TOPICO_PUB_LED_VERDE, "ON"); //confirma o status        
      } else {
        Serial.println("Led Verde - OFF");
        digitalWrite(LED_VERDE, LOW);
        MQTT.publish(TOPICO_PUB_LED_VERDE, "OFF"); //confirma o status        
      }
    }

    if (strcmp(topic, TOPICO_SUB_LED_VERMELHO) == 0) {
      if (msg == "ON") {
        Serial.println("Led Vermelho - ON");
        digitalWrite(LED_VERMELHO, HIGH);
        MQTT.publish(TOPICO_PUB_LED_VERMELHO, "ON"); //confirma o status        
      } else {
        Serial.println("Led Vermelho - OFF");
        digitalWrite(LED_VERMELHO, LOW);
        MQTT.publish(TOPICO_PUB_LED_VERMELHO, "OFF"); //confirma o status        
      }
    }

    if (strcmp(topic, TOPICO_SUB_LED_AMARELO) == 0) {
      if (msg == "ON") {
        Serial.println("Led Amarelo - ON");
        digitalWrite(LED_AMARELO, HIGH);
        MQTT.publish(TOPICO_PUB_LED_AMARELO, "ON"); //confirma o status        
      } else {
        Serial.println("Led Amarelo - OFF");
        digitalWrite(LED_AMARELO, LOW);
        MQTT.publish(TOPICO_PUB_LED_AMARELO, "OFF"); //confirma o status        
      }
    }

    Serial.print("[MQTT] Mensagem recebida: ");
    Serial.println(msg);     
}
  
void reconnect_mqtt() 
{
    while (!MQTT.connected()) 
    {
        Serial.print("* Tentando se conectar ao Broker MQTT: ");
        Serial.println(BROKER_MQTT);
        if (MQTT.connect(ID_MQTT, USER_MQTT, PASS_MQTT)) 
        {
            Serial.println("Conectado com sucesso ao broker MQTT!");
            MQTT.subscribe(TOPICO_SUB_LED_VERDE);
            MQTT.subscribe(TOPICO_SUB_LED_VERMELHO);
            MQTT.subscribe(TOPICO_SUB_LED_AMARELO);

            Serial.println("Led Verde - OFF");
            digitalWrite(LED_VERDE, LOW);
            MQTT.publish(TOPICO_SUB_LED_VERDE, "OFF"); //confirma o status desligado
            MQTT.publish(TOPICO_PUB_LED_VERDE, "OFF"); //confirma o status desligado
            
            Serial.println("Led Vermelho - OFF");
            digitalWrite(LED_VERMELHO, LOW);
            MQTT.publish(TOPICO_SUB_LED_VERMELHO, "OFF"); //confirma o status desligado
            MQTT.publish(TOPICO_PUB_LED_VERMELHO, "OFF"); //confirma o status desligado
            
            Serial.println("Led Amarelo - OFF");
            digitalWrite(LED_AMARELO, LOW);
            MQTT.publish(TOPICO_SUB_LED_AMARELO, "OFF"); //confirma o status desligado
            MQTT.publish(TOPICO_PUB_LED_AMARELO, "OFF"); //confirma o status desligado
        } 
        else
        {
            Serial.println("Falha ao reconectar no broker.");
            Serial.println("Nova tentativa de conexao em 2s");
            delay(2000);
        }
    }
}
  
void reconnect_wifi() 
{
    if (WiFi.status() == WL_CONNECTED)
        return;
         
    WiFi.begin(SSID, PASSWORD);
     
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(100);
        Serial.print(".");
    }
   
    Serial.println();
    Serial.print("Conectado com sucesso na rede ");
    Serial.printf("[WIFI]: SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}
 
void verify_connections()
{
    reconnect_wifi(); 
    if (!MQTT.connected()) 
        reconnect_mqtt(); 
} 
 
void handleTemperaturesensor(void) {
  static unsigned long last_millis;
  unsigned long        current_millis = millis();

  if (last_millis && current_millis - last_millis < EVENT_WAIT_TIME) return;
  last_millis = current_millis;
  
  temperature = dht.readTemperature();
  sprintf(temperature_string, "%2.1f", temperature);
  
  if (isnan(temperature)) {
    Serial.printf("DHT reading failed!\r\n");
    return;
  } else {
    MQTT.publish(TOPICO_PUB_TEMPERATURA_DHT11, temperature_string);
  }   

  Serial.printf("Temperature: %2.1f ºC\r\n", temperature);

  lastTemperature = temperature;
}

void loop() 
{   
    verify_connections();

    handleTemperaturesensor();
 
    MQTT.loop();

    delay(1000);   
}
