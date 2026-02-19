#include <Arduino.h>
#include "theservo.h"
#include "ultrasonic.h"
#include "LDR_sensor.h"
#include "mqtt.h"

int dist;
extern volatile bool isvalid;
extern volatile bool newmessage;
extern const char* door_unlock;
extern const char* door_person_detected;
extern const char* door_status;

void setup() {
  // put your setup code here, to run once:
  servo_pin_setup(16);
  ultrasonic_pins_setup(15, 18);
  LDR_pin_setup();
  Serial.begin(115200);
  mqtt_setup();
  close_door();  // Start with door closed
}

void loop() {
  update_led();
  static bool person_was_detected = false; // if it becomes true after a loop it stays true and does not reset to false every loopaaupdate_led();
  mqtt_loop();
  dist = ultrasonic_distance();
  if(dist < 10 && !person_was_detected){
      person_was_detected = true;
      publish(door_person_detected, "DETECTED");
      Serial.println("Person Detected");
    }
    if (newmessage) {
        newmessage = false ; // reset newmessage flag
        Serial.println("newmessage ");
      if (isvalid) {
        state = OPENING;
        publish(door_status, "OPEN");
        Serial.println("Door Opened for Valid Person");
        person_was_detected = false; 
      }
      else{
        Serial.println("error, door didnt open"); 
      }
    }
    turn_on_servo();
  }


  