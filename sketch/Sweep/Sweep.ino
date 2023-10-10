/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 https://www.arduino.cc/en/Tutorial/LibraryExamples/Sweep
*/

#include <Servo.h>

Servo servoDX;
Servo servoSX;


int pos = 0;    // variable to store the servo position

void setup() {
  servoDX.attach(9);  // attaches the servo on pin 9 to the servo object
  servoSX.attach(10);
}

void loop() {
  servoDX.write(0); //giu
  // servoDX.write(90); //su
  servoSX.write(90); //giu
  delay(2000);
  // servoDX.write(90);
  // servoSX.write(90);
  // delay(2000);
  // servoDX.write(180); //giu
    servoDX.write(90); //su
  servoSX.write(0); //su
  delay(4000);
}
