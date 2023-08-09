
// this constant won't change:
const int buttonPin = 3;  // the pin that the pushbutton is attached to
const int ledPin = 13;    // the pin that the LED is attached to

// Variables will change:
int buttonPushCounter = 0;  // counter for the number of button presses
int buttonState = 0;        // current state of the button
int lastButtonState = 0;    // previous state of the button

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(9600);
}


void loop() {

  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      buttonPushCounter++;
      Serial.println(buttonPushCounter);
    } 
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
  if (buttonPushCounter%2==0){
    Serial.println("ORARIO");
  }else{
    Serial.println("QUANTITA");
  }
}
