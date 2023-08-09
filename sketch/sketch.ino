// Librerie per il display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
/** */  
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
// Creo un oggetto display. Vengono impostate le grandezze del display,
//  viene inizializzata la libreria per comunicare tramite il bus I2C
// L'ultimo parametro specifica l'indirizzo del display oled. Con -1 prova a identificarlo automaticamente 
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/** Inizializzazione dei pin */
const int potPin = A0; 
const int switchPin = 2;
const int buttonPin = 3;

/** Definizione delle variabili */
char buffer[100]; /** buffer per la scrittura sullo schermo OLED */
int orario[2]; /** orario nel formato hh:mm */
int quantita; /** quantita di cibo da erogare [g] */ 
const int intervallo = 10; /** Passo con cui viene incrementato/decrementato l'orario */

int buttonValue = 0;      /** Segnale del pulsante */  
int lastButtonValue = 0; /** Ultimo segnale letto del pulsante*/  
int buttonPushCounter = 0;   /** Contatore che tiene traccia del numero di pressioni del tasto*/   

/** Range quantita di cibo erogabile, rispettivamente minimo e massimo */
const int lower = 150;
const int upper = 350;

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

/** Permette di impostare l'orario in cui erogare il cibo tramite il potenziometro.
 Il passo dell'orario è definito dalla variabile `intervallo` (in minuti). 
 */
void timeSetup(){
  int valorePotenziometro = analogRead(potPin);
  int timeValue = map(valorePotenziometro, 0, 1023, 0, 60/intervallo*24); // Mappa il valore da 0 a 47
  orario[0] = timeValue / (60/intervallo);
  orario[1] = (timeValue % (60/intervallo)) * intervallo;
  sprintf(buffer, "Imposta l'orario di erogazione:\n %d:%d", orario[0], orario[1]);
}

/** Permette di impostare la quantità (g) di peso da erogare tramite il potenziometro.
 Il range di valori che assume va da `lower` a `upper`. 
 */
void weightSetup(){
  int valorePotenziometro = analogRead(potPin);
  quantita = map(valorePotenziometro, 0, 1023, lower, upper);
  sprintf(buffer, "Imposta la quantita di cibo: %d", quantita);
}

/**
  Modalità che permette di modificare la quantità di cibo da erogare
  e l'orario in cui viene erogato.
  Premendo il pulsante è possibile alternare tra l'impostazione dell'orario e del peso.
*/
void setupMode(){
  buttonValue = digitalRead(buttonPin);
  if (buttonValue != lastButtonValue) {
    if (buttonValue == HIGH) {
      buttonPushCounter++;
      // Serial.println(buttonPushCounter);
    } 
    delay(50);
  }
  lastButtonValue = buttonValue;
  if (buttonPushCounter%2==0){
    timeSetup();
  }else{
    weightSetup();
  }
}


void loop() { 
  int switchValue = digitalRead(switchPin);
  display.clearDisplay();
  display.setCursor(0, 0);
  if (switchValue == HIGH) setupMode();
   else {
    sprintf(buffer, "Quantita cibo: %d, orario %d:%d\n", quantita, orario[0], orario[1]);
  }
  display.println(buffer);
  display.display();
  
}
