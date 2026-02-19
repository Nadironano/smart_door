#ifndef MQTT_H
#define MQTT_H

void mqtt_setup();
void mqtt_loop();
void publish(const char* topic, const char* a_message);
extern volatile bool isvalid;
extern volatile bool newmessage;
extern const char* door_unlock;
extern const char* door_person_detected;
extern const char* door_status;

#endif 