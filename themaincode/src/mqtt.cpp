#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "mqtt.h"

const char* ssid = "CELEC";
const char* password = "CELEC@87";
const char* mqtt_server = "192.168.1.141"; // PC IP
const int mqtt_port = 1883;
const char* door_person_detected = "door/person_detected"; // ESP32 publishes to pc when person is detected
const char* door_unlock = "door/unlock"; // pc publishes to esp32 VALID or INVALID
const char* door_status = "door/status"; // ESP32 publishes to pc door status OPEN or CLOSED
const unsigned long wifiWaitInterval = 1000;
const unsigned long mqttWaitInterval = 5000; 
unsigned long wifiWaitStartTime = 0;
unsigned long mqttWaitStartTime = 0;
volatile bool isvalid = false; // global variable to track if person is valid, volatile as it is changed when interrupted
volatile bool newmessage = false; // global variable to track new message received
WiFiClient espClient;
PubSubClient client(espClient);
void mqttCallback(char* topic, byte* payload, unsigned int length);



void mqtt_setup() {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  wifiWaitStartTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();
    if (now - wifiWaitStartTime >= wifiWaitInterval){
      Serial.print(".");  // Just print dots like typical Arduino examples
      wifiWaitStartTime = now;
    }
  }
  Serial.println("\nWi-Fi connected!");
  Serial.println("\nWi-Fi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP()); // <-- confirm ESP32 IP

  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(mqttCallback);
}

void mqtt_loop() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi lost, reconnecting...");
    WiFi.begin(ssid, password);
    wifiWaitStartTime = millis();
    return;
  }
  unsigned long now = millis();
  if (!client.connected()) {
    if (now - mqttWaitStartTime >= mqttWaitInterval){
      Serial.print("Attempting MQTT connection...");
      if (client.connect("ESP32Client")) {
        Serial.println("connected");
        client.subscribe(door_unlock);
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
      }
      mqttWaitStartTime = now;
      }
  } else { // if client connected
    client.loop(); // handle incoming messages
    }
  }
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // put your mqtt callback code here: you dont need to add it to main.cpp or mqtt.h
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  if(String(topic) == door_unlock){
    if(message == "VALID"){
        isvalid = true;
        newmessage = true;
        Serial.println("Person Validated");
      }
    else{
        isvalid = false;
        newmessage = true;
        Serial.println("Access Denied - Person Invalid");
    }
  }
}

void publish(const char* topic, const char* a_message) {
  // put your publish code here:
    client.publish(topic,a_message);
    Serial.print("Published to topic: ");
    Serial.println(topic);
}
