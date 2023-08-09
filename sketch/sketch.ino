// Librerie per il display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// Creo un oggetto display. Vengono impostate le grandezze del display,
//  viene inizializzata la libreria per comunicare tramite il bus I2C
// L'ultimo parametro specifica l'indirizzo del display oled. Con -1 prova a identificarlo automaticamente 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

const int potPin = A0; // Pin analogico a cui è collegato il potenziometro
const int switchPin = 2;
const int buttonPin = 3;
char buffer[100];
int quantita;
int orario[2];

int buttonState = 0;        
int lastButtonState = 0; 
int buttonPushCounter = 0;    



//  Range peso pasto, rispettivamente minimo e massimo
 int lower = 150;
 int upper = 350;

void setup() {                                          
  Serial.begin(9600); // Inizializza la comunicazione seriale a 9600 bps
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // L'indirizzo I2C del display potrebbe essere diverso (0x3C è l'indirizzo più comune)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
 
}

void timeSetup(){
  int intervalloMinuti = 10;
  int valorePotenziometro = analogRead(potPin);
  int timeValue = map(valorePotenziometro, 0, 1023, 0, 60/intervalloMinuti*24); // Mappa il valore da 0 a 47
  
  // Calcola le ore e i minuti in base all'impostazione corrente
  orario[0] = timeValue / (60/intervalloMinuti);
  orario[1] = (timeValue % (60/intervalloMinuti)) * intervalloMinuti;
  sprintf(buffer, "Imposta l'orario di erogazione:\n %d:%d", orario[0], orario[1]);

    // Serial.println("ORARIO");

}

void weightSetup(){
  int valorePotenziometro = analogRead(potPin);
  quantita = map(valorePotenziometro, 0, 1023, lower, upper);
  // Serial.println("Q");
  sprintf(buffer, "Imposta la quantita di cibo: %d", quantita);
  

    
}

void setupMode2(){
  buttonState = digitalRead(buttonPin);
  if (buttonState != lastButtonState) {
    if (buttonState == HIGH) {
      buttonPushCounter++;
      Serial.println(buttonPushCounter);
    } 
    delay(50);
  }
  lastButtonState = buttonState;
  if (buttonPushCounter%2==0){
    timeSetup();
  }else{
    weightSetup();
  }
}

// int setupMode(){
//     int valorePotenziometro = analogRead(potPin);
//     // int buttonCounter = 0;
//     // int buttonState = digitalRead(buttonPin);        // current state of the button
//     // int lastButtonState = 0; 
//     // if (buttonState != lastButtonState) {
//     //   if (buttonState == HIGH) 
//     //     buttonCounter++;
//     //   lastButtonState = buttonState;
//     //   }

//     // Serial.println(buttonCounter);
//     if (buttonCounter%2==0){
//       quantita = map(valorePotenziometro, 0, 1023, lower, upper);
//     }else{
//       int hh, mm;
//       hh, mm = timeSetup(valorePotenziometro);
      
//     }
//     sprintf(buffer, "--- Setup ---\nInserisci la quantità di cibo da erogare: %dg", quantita);
//     return quantita;
// }


void loop() { 
  int switchValue = digitalRead(switchPin);
  display.clearDisplay();
  display.setCursor(0, 0);
  
  if (switchValue == HIGH) {
    setupMode2();
  } else {
    sprintf(buffer, "Quantita cibo: %d, orario %d:%d\n", quantita, orario[0], orario[1]);
  }
  display.println(buffer);
  display.display();
  delay(50);
}
