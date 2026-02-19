#include <Arduino.h>
#include <ESP32Servo.h>
#include "theservo.h"

static Servo myservo;

const unsigned long waitInterval = 1000;
unsigned long waitStartTime = 0;


ServoState state = IDLE;

void servo_pin_setup(int servo_pin) {
  ESP32PWM::allocateTimer(0);
  myservo.setPeriodHertz(50);
  myservo.attach(servo_pin, 500, 2500);
}

void turn_on_servo() {
  unsigned long now = millis();

  switch (state) {
    case IDLE:
      break;

    case OPENING:
      myservo.write(0);
      waitStartTime = now;
      state = WAIT_OPEN;
      break;

    case WAIT_OPEN:
      if (now - waitStartTime >= waitInterval) {
        state = CLOSING;
      }
      break;

    case CLOSING:
      myservo.write(48);
      waitStartTime = now;
      state = WAIT_CLOSE;
      break;
    case WAIT_CLOSE:
      if (now - waitStartTime >= waitInterval) {
        // do nothing, remain in WAIT_CLOSE state
        state = IDLE;
      }
      break;
  }
}
void close_door() {
  myservo.write(48);
}
