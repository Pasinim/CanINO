#include <HX711.h>
#include <Servo.h>


const int data_loadCell = 6;
const int clock_loadCell = 7;
float calibration_factor = 2017.817626; /** Valore di calibrazione per la cella di carico*/ 
float offset_hx711 = 268839; /** Offset della cella di carico */ 
HX711 scale; /** Variabile di istanza per utilizzare il modulo HX711*/

Servo servoDX;
Servo servoSX;


void setup() {
  Serial.begin(9600);
  servoDX.attach(9);  
  servoSX.attach(10);
  servoDX.write(90); 
  servoSX.write(0); 
  scale.begin(data_loadCell, clock_loadCell);
  scale.set_offset(offset_hx711); 
  scale.set_scale(calibration_factor);
}


void openServo () {
  servoDX.write(90); 
  servoSX.write(0); 
  Serial.println("Apri");
}

void closeServo () {
  servoSX.write(90); 
  servoDX.write(0); 
  Serial.println("Chiudi");
}

void eroga(int q, float tara){
  while (scale.get_units(5) - tara < q){
    Serial.print("Peso - tara: ");
    Serial.println(scale.get_units(5) - tara);
    openServo();
    delay(50);
  }
  closeServo(); 
}

void loop() {



  // scale.tare()
  //   Serial.print("get value: \t\t");
  // Serial.println(scale.get_value(5));		// print the average of 5 readings from the ADC minus the tare weight, set with tare()
  float tara = 217.9;
  Serial.print("get units: \t\t");
  float w = scale.get_units(5) - tara;
  Serial.println(w);     
  // put your main code here, to run repeatedly:
  eroga(7, tara);
}
