#include <Stepper.h>

// Defines the number of steps per rotation
const int stepsPerRevolution = 2048;
const int quarter_turn = 512 ;
const int buttonPin = 2; 
int buttonState = 0;
int calibration = 0;
//calibration is the messege from the base to calibrate
bool position = false;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
    pinMode(buttonPin, INPUT);
    Serial.begin(9600);
}

void loop() {
  buttonState = digitalRead(buttonPin);
  calibration = Serial.available();
  if (calibration != 0){    // calibration messege to be sent from base station 
    if (buttonState == 0){
      myStepper.setSpeed(10);
      myStepper.step(1);
    }
    else if (buttonState == 1){
      myStepper.setSpeed(10);
      myStepper.step(0);
      position = true;
    }
  }
}
