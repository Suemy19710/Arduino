#include <Stepper.h>

const int stepsPerRevolution = 1048;
const int rolePerMinute = 15;
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

void setup() {
  myStepper.setSpeed(rolePerMinute);
  Serial.begin(9600);
}

void loop(){
  Serial.println("clockwise");
  myStepper.step(2048);
  delay(500);

  Serial.println("counterclockwise");
  myStepper.step(-2048);
  delay(500);
}
