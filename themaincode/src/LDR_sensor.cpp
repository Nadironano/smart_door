#include <Arduino.h>
#include "LDR_sensor.h"

// you dont need to make the whole room dark to test this sensor, only cover the LDR sensor with your hand or a piece of paper

static int LDR_pin = 5;
int LED_pin = 17; 

void LDR_pin_setup() {
  pinMode(LDR_pin, INPUT);
  pinMode(LED_pin, OUTPUT);
}

// Update LED based on LDR reading
void update_led() {
  int LDR_value = digitalRead(LDR_pin);
  if(LDR_value == HIGH) { // Assuming HIGH means Dark
    digitalWrite(LED_pin, HIGH); // turn on when Dark
  } else {
    digitalWrite(LED_pin, LOW);  // turn off when Light
  }
}

