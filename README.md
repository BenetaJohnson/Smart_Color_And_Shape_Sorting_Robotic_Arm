# Gesture-Based Robotic Arm Control with Color & Shape Detection

This project uses a Raspberry Pi with a camera module to detect the color and shape of objects and communicates the detected data to an Arduino-controlled robotic arm that picks and sorts objects accordingly.

---

## Components

- Raspberry Pi with Camera Module Rev 1.3  
- Arduino Uno/Mega  
- Robotic Arm with Servos  
- Serial connection between Raspberry Pi and Arduino  

---

## Features

- Real-time detection of object color (Red, Blue, Green) and shape (Circle, Square).  
- Communication from Raspberry Pi to Arduino via serial port in the format `<Color,Shape>`.  
- Arduino controls servos to pick up, move, and drop objects based on detected parameters.  
- Timeout and retry mechanism in Arduino for communication robustness.  

---

## Setup

### Raspberry Pi

1. Enable camera: `sudo raspi-config` → Interface Options → Camera → Enable → Reboot  
2. Install dependencies:  
sudo apt update
sudo apt install python3-opencv python3-pip
pip3 install numpy pyserial
3. Connect camera and run the detection script (to be created) that sends detected color and shape data over serial in the format `<Color,Shape>`.  

### Arduino

1. Connect servos to pins as per the Arduino code.  
2. Upload the provided Arduino sketch to Arduino.  
3. Connect Arduino and Raspberry Pi serial ports properly (with level shifting if needed).  
4. The Arduino will request data every 5 seconds and respond accordingly to move the robotic arm.

---

## Usage

- Power on Raspberry Pi and Arduino.  
- Run the detection Python script on Raspberry Pi to detect and send object info.  
- The Arduino will pick and place objects based on received commands.

---

## Notes

- Modify servo angles and timing in Arduino code to fit your robotic arm.  
- Modify detection thresholds in Raspberry Pi code for lighting conditions.  
- Ensure serial baud rates match on both devices.

---

---
