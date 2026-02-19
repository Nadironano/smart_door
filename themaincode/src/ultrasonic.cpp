
#include <Arduino.h>
#include "ultrasonic.h"

static int trig; // static means global
static int echo; 

unsigned long currentTime = 0;
unsigned long previousTime = 0;
const unsigned long interval = 60; // milliseconds

int distance = 180;

void ultrasonic_pins_setup(int trigPin, int echoPin){
  trig = trigPin;
  echo = echoPin;
  pinMode(echo, INPUT);
  pinMode(trig, OUTPUT);
}

int ultrasonic_distance(){
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= interval) {
  previousTime = currentTime;

  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseInLong(echo, HIGH, 30000); // timeout after 30ms if nothing received

  if (duration == 0) distance = 180; // if nothing detected set to max range
  else distance = duration * 0.034 / 2; // calculate distance in cm
}
return distance;
}