
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"
#include "RTClib.h"
#include <Servo.h>
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
const int data_loadCell = 6;
const int clock_loadCell = 7;

/** Definizione delle variabili */
char buffer[100]; /** buffer per la scrittura sullo schermo OLED */
int orario[2]; /** orario nel formato hh:mm */
int quantita; /** quantita di cibo da erogare [g] */ 
const int intervallo = 10; /** Passo con cui viene incrementato/decrementato l'orario */

int buttonValue = 0;      /** Segnale del pulsante */  
int lastButtonValue = 0; /** Ultimo segnale letto del pulsante*/  
int buttonPushCounter = 0;   /** Contatore che tiene traccia del numero di pressioni del tasto*/   

float calibration_factor = 2017.817626; /** Valore di calibrazione per la cella di carico*/ 
float offset_hx711 = 268839; /** Offset della cella di carico */ 
HX711 scale; /** Variabile di istanza per utilizzare il modulo HX711*/
RTC_DS1307 rtc;  /** Variabile di istanza per utilizzare il modulo rtc*/
Servo servoDX; /** Variabili per il Servo 1 **/
Servo servoSX;
/** Range quantita di cibo erogabile, rispettivamente minimo e massimo */
const int lower = 150;
const int upper = 350;

/** 
Metodo utile per il debugging
*/
void debug(){
  // char buffer[50];
  // Stampo il valore della SetupMode();
  sprintf(buffer, "Valore switch: %s", digitalRead(switchPin) == HIGH ? "SetupMode attiva" : "SetupMode NON attiva");
  Serial.println(buffer);
   if (buttonValue == LOW) {
      Serial.println("Tasto premuto");
    } 
    DateTime time = rtc.now();
   Serial.println(time.timestamp(DateTime::TIMESTAMP_TIME));
  delay(500);
} 

/** ------------------------------------------------------------------------------------ */
void setup() {                                          
  Serial.begin(9600); 
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); 
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  servoDX.attach(9);
  servoSX.attach(10);
  servoDX.write(0);
  servoSX.write(90);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  scale.begin(data_loadCell, clock_loadCell);
  scale.set_offset(offset_hx711); 
  scale.set_scale(calibration_factor);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (! rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void timeSetup(){
  int valorePotenziometro = analogRead(potPin);
  int timeValue = map(valorePotenziometro, 0, 1023, 0, 60/intervallo*24); // Mappa il valore da 0 a 47
  orario[0] = timeValue / (60/intervallo);
  orario[1] = (timeValue % (60/intervallo)) * intervallo;
  sprintf(buffer, "Imposta l'orario di erogazione: %d:%d \n", orario[0], orario[1]);
  DateTime orarioErogazione(rtc.now().year(), rtc.now().month(), rtc.now().day(), orario[0], orario[1], 0);
}

/** Permette di impostare la quantità (g) di peso da erogare tramite il potenziometro.
 Il range di valori che assume va da `lower` a `upper`. 
 */
void weightSetup(){
  int valorePotenziometro = analogRead(potPin);
  quantita = map(valorePotenziometro, 0, 1023, lower, upper);
  sprintf(buffer, "Imposta la quantita di cibo: %d", quantita);
}

void setupMode(){
  Serial.println("SETUP ATTIVO");
  buttonValue = digitalRead(buttonPin);
  if (buttonValue != lastButtonValue) {
    if (buttonValue == HIGH) {
      buttonPushCounter++;
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

void loop(){
  DateTime time = rtc.now();
  display.setCursor(0, 0);
  display.clearDisplay();
  display.println("Ciao");
  display.display();
  int switchValue = digitalRead(switchPin);
  Serial.println(switchValue);
  Serial.println(buttonValue);
    if (switchValue == HIGH) setupMode();
   else {
    sprintf(buffer, "\t\t\t\t\t %02d:%02d:%02d \t\t\t\t\t\n\nVerranno erogati %d grammi di cibo alle\n  %02d:%02d\n", time.hour(), time.minute(), time.second(), quantita, orario[0], orario[1]);
  }
  //  if (switchValue == HIGH) {
  //   display.println(switchValue);
  //   display.display();
  //   setupMode();
  //  }
  //  else {
  //   sprintf(buffer, "\t\t\t\t\t %02d:%02d:%02d \t\t\t\t\t\n\nVerranno erogati %d grammi di cibo alle\n  %02d:%02d\n", time.hour(), time.minute(), time.second(), quantita, orario[0], orario[1]);
  // }
  // //se metto queste due righe nell'else non stampa correttamente quando entra in setupMode()
  // Serial.println(buffer);
  // display.println(buffer);
  // display.display();
}