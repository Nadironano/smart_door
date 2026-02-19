#ifndef THESERVO_H
#define THESERVO_H

enum ServoState {
  OPENING,
  WAIT_OPEN,
  CLOSING,
  WAIT_CLOSE,
  IDLE
};

extern ServoState state;
void servo_pin_setup(int servo_pin_);
void turn_on_servo();
void close_door();

#endif 