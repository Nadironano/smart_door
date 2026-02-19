# first the face is detected then cropped then resized then it is converted to black and white 
# then we convert certain parts of the face into numbers creating a numpy array then a vector 
# then we calculate the distance between the two vectors if the distance is less than a certain threshold we say its a match 
# otherwise we say its not a match
# this is code is antispoofing 
import cv2
from deepface import DeepFace
import numpy as np
import os
import paho.mqtt.client as mqtt  
ssid = "CELEC"
password = "CELEC@87"
mqtt_server = "192.168.1.141"
mqtt_port = 1883
door_unlock = "door/unlock"
door_person_detected = "door/person_detected"
from threading import Event
person_detected_event = Event()
client = mqtt.Client(client_id="PC_FACE_RECOGNITION")


# The callback for when the PC receives a response from the broker.
def on_connect(client, userdata, flags, rc):
    if rc == 0:
        client.subscribe(door_person_detected)
        print("Connected to MQTT Broker!")
    else:
        print(f"Failed to connect, return code {rc}")
# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, message):
    if message.topic == door_person_detected:
        payload = message.payload.decode().strip()
        if payload == "DETECTED":
            print("Person detected")
            person_detected_event.set()
           
        
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(mqtt_server, mqtt_port, 60)
client.loop_start()
DeepFace.build_model("VGG-Face")  # Pre-loads the model


    
def face_recognition():    
    refrencefolder = "D:/references/"
    process_every_n_frames = 5
    spoof_every_n_frames = 30
    spoof_result = True
    threshhold = 0.35
    camera_width = 640
    camera_height = 480
    img_emmbeddings = []

    # function to check if the face is real or spoofed, anti-spoofing
    def is_real_face(face_img):
        result = DeepFace.extract_faces(
            img_path=face_img,
            enforce_detection=False,
            anti_spoofing=True
        )
        return result[0]["is_real"]

    # check if reference folder exists
    if not os.path.exists(refrencefolder):
        print("ERROR: Reference folder not found!")
        exit(1)
        
    # turn on the camera then crop the face    
    cap = cv2.VideoCapture(0, cv2.CAP_DSHOW)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, camera_width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, camera_height)

    if not cap.isOpened():
        print("ERROR: Could not open camera!")
        exit(1)

    # read the files
    for img in os.listdir(refrencefolder):
        if img.lower().endswith(('.png', '.jpg', '.jpeg')):
            try:
                img_path = os.path.join(refrencefolder, img)
                # Use VGG-Face model with cosine metric for the pictures in the file, it returns a vector of numbers, [0]["embedding"] gets only the embedding part not the whole dictionary
                embedding = DeepFace.represent(img_path, model_name="VGG-Face", enforce_detection=False, detector_backend="opencv")[0]["embedding"]
                img_emmbeddings.append(np.array(embedding)) # convert to numpy array
            except Exception as e:
                print(f"Error loading {img}: {e}")

    count_frames = 0
    status = "Initializing..."  # Initialize status
    color = (255, 255, 255)     # Initialize color

    # function to calculate cosine distance between two vectors
    def cosine_distance(a, b):
        # we use numpy to calculate the dot product over the magnitude of the vectors minus 1  
        return 1 - np.dot(a,b) / (np.linalg.norm(a) * np.linalg.norm(b)) 

    while True:
        # read frames from the camera 
        ret, frame = cap.read() 
        if not ret:
            print("ERROR: Could not read frame from camera!")
            break
        count_frames += 1
        if count_frames % process_every_n_frames == 0:
            try :
                faces = DeepFace.extract_faces(img_path=frame,enforce_detection=False,detector_backend="opencv")
                if not faces:
                    status = "No face detected"
                    color = (255, 255, 0)
                    continue
                if count_frames % spoof_every_n_frames == 0:
                    spoof_result = is_real_face(frame)
                if not spoof_result:
                    status = "Spoof Detected"
                    color = (0, 0, 255)  # Red
                else:    
                    # Use VGG-Face model with cosine metric for the pictures in the video, it returns a vector of numbers
                    result = DeepFace.represent(frame, model_name="VGG-Face", enforce_detection=False, detector_backend="opencv")
                    # check if the model detected a face in the frame, if not it will return an empty list which is less than 1
                    if result and len(result) > 0:
                        live_embedding = np.array(result[0]["embedding"])  # convert to numpy array, [0]["embedding"] gets only the embedding part not the whole dictionary
                        # a list that contains the comparison values of the live embedding distance with reference embedding for each image in the reference folder
                        distances = [cosine_distance(live_embedding, img_emmbedding) for img_emmbedding in img_emmbeddings] # [] are for the list
                        # Get the minimum distance between the live embedding and reference embeddings
                        min_distance = min(distances)
                        # Determine if access is granted or denied based on the threshold
                        if min_distance < threshhold:
                            status = "Access Granted"
                            client.publish(door_unlock, "VALID", qos=1)
                            color = (0, 255, 0)  # Green
                            break  # Exit the loop after granting access
                        else:
                            status = "Access Denied"
                            color = (0, 0, 255)  # Red
                    else:
                        status = "No face detected"
                        color = (255, 255, 0)  # Yellow
            except Exception as e:
                # Display "No face detected" on frame
                cv2.putText(
                    frame,
                    "No face detected",
                    (20, 75),
                    cv2.FONT_HERSHEY_SIMPLEX,
                    1,
                    (255, 255, 0),  # Yellow
                    2
                    )
        # Display the frame with status text and color
        cv2.putText(
            frame,                     # image
            status,                    # text to display
            (20, 75),                  # position (x, y)
            cv2.FONT_HERSHEY_SIMPLEX,  # font
            1,                         # font scale
            color,                     # color (B, G, R)
            2                          # thickness
                )

        if cv2.waitKey(10) == 27:
            break  

        cv2.imshow("Face Recognition", frame)

    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    try:
        while True:
            person_detected_event.wait()   # sleep until DETECTED
            if person_detected_event.is_set():
                face_recognition()
                person_detected_event.clear()
    except KeyboardInterrupt:
        pass
    finally:
        client.loop_stop()
        client.disconnect()               