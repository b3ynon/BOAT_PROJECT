//Includes the Arduino Stepper Library
#include <Stepper.h>
int Movement;

// Defines the number of steps per rotation
const int stepsPerRevolution = 2048;
const int quarter_turn = 512 ;
const int pump = 30;

// Creates an instance of stepper class
// Pins entered in sequence IN1-IN3-IN2-IN4 for proper step sequence
Stepper myStepper = Stepper(stepsPerRevolution, 13, 10, 12, 11);

void setup() {
  Serial.begin(9600);
  pinMode(pump, OUTPUT);
  digitalWrite(pump, LOW);
  Serial.println("ready");
}

void loop() {
  if (Serial.available() > 0) {
    Movement = Serial.read();
    if (Movement == '1'){
      CalibrationToPin();      // Function called to spin motor until end stop is reached
    }
    
    else if (Movement == '2'){      // '2' by serial calls a water sample to be taken and rotation of holder 
      QuarterTurn();
      PumpOn();
    }
  }   
}

void CalibrationToPin (){
  myStepper.setSpeed(3);
  myStepper.step(-2000);
  delay(1000);
}

void QuarterTurn (){
  myStepper.setSpeed(5);
  myStepper.step(quarter_turn);
  delay(1000);
}

void PumpOn (){
  digitalWrite(pump, HIGH);
  delay(10000);
  digitalWrite(pump, LOW);
}
