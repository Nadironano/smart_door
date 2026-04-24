🚪 Smart Face-Recognition Door Lock (ESP32-S3 & DeepFace), IoT-based access control system.

🚀 How It Works 
Sensing: The HC-SR04 Ultrasonic Sensor monitors for movement within 10cm.
Trigger: The ESP32-S3 publishes a DETECTED message via MQTT.
Analysis: A Python service on the PC wakes up, captures a camera frame, and performs:
Anti-Spoofing: Ensures the face is a real human, not a photo/screen.Encoding: Converts the face into a mathematical vector (Embedding).Matching: Calculates the Cosine Distance between the live vector and saved references.
Action: If a match is found ($Distance < 0.35$), the PC sends a VALID command. The ESP32-S3 then triggers the Servo Motor to unlock the door.

🛠️ Tech StackMicrocontroller: 
ESP32-S3 (Programmed via PlatformIO).
Framework: Arduino (C++) & Python 3.
AI Libraries: DeepFace (VGG-Face model), OpenCV, NumPy.
Communication: MQTT (Paho-MQTT on PC, PubSubClient on ESP32).
Hardware: SG90 Servo, HC-SR04 Ultrasonic Sensor, LDR Sensor.

📂 Modular Firmware Structure (ESP32-S3)The firmware is built using a header-based approach for high maintainability:theservo.h: PWM control logic for the locking mechanism.ultrasonic.h: Timing logic for distance measurement.mqtt.h: Managed Wi-Fi connection and asynchronous MQTT callback handling.LDR_sensor.h: Environmental light sensing for visual feedback.
