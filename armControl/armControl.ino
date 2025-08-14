#include <Servo.h>

Servo baseServo;
Servo shoulderServo;
Servo armServo;
Servo leftFinger;
Servo rightFinger;

int currentPos;
String inputString = "";  // A string to hold incoming data
boolean stringComplete = false;  // Whether the string is complete
unsigned long lastRequestTime = 0;  // Track time of last request
unsigned long timeoutDuration = 5000;  // Timeout after 5 seconds (5000 ms)

void setup() {
  Serial.begin(9600);  // Start serial communication at baud rate 9600

  // Now attach servos without the sudden fast movement
  baseServo.write(40);
  baseServo.attach(11);
  delay(1000);
  shoulderServo.write(23);
  shoulderServo.attach(10);
  delay(1000);
  armServo.write(0);
  armServo.attach(9);
  delay(1000);
  rightFinger.write(40);
  rightFinger.attach(6);
  delay(2000);
}

void moveTo(Servo myServo, int angle) {
  currentPos = myServo.read();
  if (currentPos < angle) {
    for (int pos = currentPos; pos <= angle; pos++) {
      myServo.write(pos);  // Move to the next position
      delay(50);  // Adjust delay for speed (higher = slower)
    }
  } else if (currentPos > angle) {
    for (int pos = currentPos; pos >= angle; pos--) {
      myServo.write(pos);  // Move to the next position
      delay(50);  // Adjust delay for speed (higher = slower)
    }
  }
  currentPos = angle;
}

void returnToOriginalPosition() {
  moveTo(shoulderServo, 5);
  moveTo(baseServo, 40);
  moveTo(rightFinger, 35);
  moveTo(armServo, 0);
  moveTo(shoulderServo, 35);

}

void pickUp() {
  moveTo(rightFinger, 45);
  moveTo(shoulderServo, 35);
  moveTo(rightFinger, 0);
  moveTo(shoulderServo, 3);
}

void drop() {
  moveTo(shoulderServo, 7);
  moveTo(rightFinger, 45);
  // moveTo(armServo,0);
}

void loop() {
  // Check if the timeout has passed without receiving a response
  if (millis() - lastRequestTime >= timeoutDuration) {
    Serial.println("REQUEST_COLOR");  // Send a request to the Raspberry Pi for the color
    lastRequestTime = millis();  // Update the time of the last request
  }

  // Check if serial data is available and handle it
  if (Serial.available() > 0) {
    serialEvent();  // Only call serialEvent if there is data available
  }
  
  // Process the received color
  if (stringComplete) {
    Serial.print("Received Color,Shape: ");
    Serial.println(inputString);
    
    if (inputString == "Red,Circle") {
      pickUp();
      moveTo(baseServo, 70);
      moveTo(armServo,30);
      drop();
      returnToOriginalPosition();
    }
    else if (inputString == "Red,Square") {
      pickUp();
      moveTo(baseServo, 85);
      moveTo(armServo,40);
      drop();
      returnToOriginalPosition();
    }
    else if (inputString == "Blue,Circle") {
      pickUp();
      moveTo(baseServo, 120);
      drop();
      returnToOriginalPosition();
    }
    else if (inputString == "Blue,Square") {
      pickUp();
      moveTo(baseServo, 100);
      moveTo(armServo,7);
      drop();
      returnToOriginalPosition();
    }
    else if (inputString == "Green,Circle") {
      pickUp();
      moveTo(baseServo, 145);
      moveTo(armServo,20);
      drop();
      returnToOriginalPosition();
    }
    else if (inputString == "Green,Square") {
      pickUp();
      moveTo(baseServo, 167);
      moveTo(armServo,10);
      drop();
      returnToOriginalPosition();
    }

    // Clear the string for the next request
    inputString = "";
    stringComplete = false;

    // Add a delay before requesting again
    delay(3000);  // Adjust the delay as needed
  }
}

// Function to handle serial data
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    
    if (inChar == '<') {
      inputString = "";  // Start reading a new message
    } else if (inChar == '>') {
      stringComplete = true;  // Mark the string as complete
      lastRequestTime = millis();  // Reset the request time once a response is received
    } else {
      inputString += inChar;  // Append character to string
    }
  }
}
