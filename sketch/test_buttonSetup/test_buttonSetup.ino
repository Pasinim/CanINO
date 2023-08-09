#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
const int switchPin = 2;  // the pin that the pushbutton is attached to


int switchValue = 0;        
void setup() {
 Serial.begin(9600); // Inizializza la comunicazione seriale a 9600 bps
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // L'indirizzo I2C del display potrebbe essere diverso (0x3C è l'indirizzo più comune)
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);  
  // initialize the LED as an output:
  pinMode(switchPin, INPUT_PULLUP);
  Serial.begin(9600);
}

void setupMode(){
    display.println("SONO IN SETUP");
    display.println(switchValue);
    return;  
}

void loop() {
  switchValue = digitalRead(switchPin);
  display.clearDisplay();
  display.setCursor(0, 0);
  if (switchValue == HIGH) {
    setupMode();
  }else{
    display.println(switchValue);
  }
  delay(100);
  display.display();

    
}
