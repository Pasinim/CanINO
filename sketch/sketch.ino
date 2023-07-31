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
int valorePotenziometro;

void setup() {
  Serial.begin(9600); // Inizializza la comunicazione seriale a 9600 bps
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // L'indirizzo I2C del display potrebbe essere diverso (0x3C è l'indirizzo più comune)
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  
}

void loop() {
  char buffer[100];
  int vPotenziometro = analogRead(potPin); // Legge il valore del potenziometro (da 0 a 1023)
  // Serial.println("Valore potenziometro: ");
  display.clearDisplay();
  display.setCursor(0, 0);
  sprintf(buffer, "Peso: \n%d g\n", vPotenziometro);
  display.println(buffer);
  display.display();
  delay(500); // Aggiungi un piccolo ritardo per evitare letture troppo veloci
}
