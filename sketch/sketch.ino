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

int setupMode(){
    int valorePotenziometro = analogRead(potPin);
    quantita = map(valorePotenziometro, 0, 1023, lower, upper);
    sprintf(buffer, "--- Setup ---\nInserisci la quantità di cibo da erogare: %dg", quantita);
    return quantita;
}


void loop() { 
  int switchValue = digitalRead(switchPin);
  display.clearDisplay();
  display.setCursor(0, 0);
  
  if (switchValue == HIGH) {
    quantita = setupMode();
  } else {
    sprintf(buffer, "Quantita cibo: %d\n", quantita);

  }
  display.println(buffer);
  display.display();
  delay(50);
}
