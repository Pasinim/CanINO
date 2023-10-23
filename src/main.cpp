// Librerie per il display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <HX711.h>
#include <RTClib.h>
#include <Servo.h>

/** */  
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Creo un oggetto display. Vengono impostate le grandezze del display,
//  viene inizializzata la libreria per comunicare tramite il bus I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

/** Inizializzazione dei pin */
const int potPin = A0; 
const int switchPin = 2;
const int buttonPin = 3;
const int data_loadCell = 6;
const int clock_loadCell = 7;

/** Definizione delle variabili */
int orario[2]; /** orario nel formato hh:mm */
int quantita; /** quantita di cibo da erogare [g] */ 
const int intervallo = 10; /** Passo con cui viene incrementato/decrementato l'orario (in minuti) */

int buttonValue = 0;      /** Segnale del pulsante */  
int lastButtonValue = 0; /** Ultimo segnale letto del pulsante*/  
int buttonPushCounter = 0;   /** Contatore che tiene traccia del numero di pressioni del tasto*/   

float calibration_factor = 2017.817626; /** Valore di calibrazione per la cella di carico*/ 
float offset_hx711 = 268839; /** Offset della cella di carico */ 
HX711 scale; /** Variabile di istanza per utilizzare il modulo HX711*/
Servo servoDX;
Servo servoSX;
RTC_DS1307 rtc;  /** Variabile di istanza per utilizzare il modulo rtc*/

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
  servoSX.attach(9);
  servoDX.attach(10);
  servoSX.write(90); 
  servoDX.write(0);
  pinMode(switchPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  scale.begin(data_loadCell, clock_loadCell);
  scale.set_offset(offset_hx711); 
  scale.set_scale(calibration_factor);
  scale.tare();

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }
  if (! rtc.isrunning()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

/** Restituisce l'offset necessario per centrare la stringa sulle x del display OLED */
int centerDisplay(char s[]){
  return (SCREEN_WIDTH - strlen(s) * 6) / 2;
}

/** Permette di impostare l'orario in cui erogare il cibo tramite il potenziometro.
 Il passo dell'orario è definito dalla variabile `intervallo` (in minuti). 
 */
void timeSetup(){
  Serial.println("Time setup");
  int valorePotenziometro = analogRead(potPin);
  int timeValue = map(valorePotenziometro, 0, 1023, 0, 60/intervallo*24); // Mappa il valore da 0 a 47
  orario[0] = timeValue / (60/intervallo);
  orario[1] = (timeValue % (60/intervallo)) * intervallo;

/** Stampa su display **/
  char str[50]; 
  strcpy(str, "SETUP");
  display.setCursor(centerDisplay(str), 0);
  display.println(str);
  strcpy(str, "ORARIO");
  display.setCursor(centerDisplay(str), 8);
  display.println(str);
  strcpy(str, "Imposta l'orario di \nerogazione: ");
  display.setCursor(centerDisplay(str), 18);
  display.print(str);
  sprintf(str, "%02d:%02d", orario[0], orario[1]);
  display.println(str);

  /** Definisco la variabile orarioErogazione: yyyy/mm/gg, orario[0]:orario[1]:00;  **/
  DateTime orarioErogazione(rtc.now().year(), rtc.now().month(), rtc.now().day(), orario[0], orario[1], 0);
  display.display();
}


/** Permette di impostare la quantità (g) di peso da erogare tramite il potenziometro.
 Il range di valori che assume va da `lower` a `upper`. 
 */
void weightSetup(){
  Serial.println("weight setup");
  int valorePotenziometro = analogRead(potPin);
  quantita = map(valorePotenziometro, 0, 1023, lower, upper);

/** Stampa su display **/
  char str[50]; 
  strcpy(str, "SETUP");
  display.setCursor(centerDisplay(str), 0);
  display.println(str);
  strcpy(str, "QUANTITA'");
  display.setCursor(centerDisplay(str), 8);
  display.println(str);
  strcpy(str, "Imposta la quantita\n di cibo: ");
  display.setCursor(centerDisplay(str), 18);
  display.print(str);
  sprintf(str, "%d", quantita);
  display.println(str);
  display.display();
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
    } 
    delay(50);
  }
  lastButtonValue = buttonValue;
  if (buttonPushCounter%2==0)
    timeSetup();
  else
    weightSetup();
}

/**
* Restiutuisce true se l'orario impostato corrisponde all'orario attuale
**/
bool checkTime(int o[]){ //da togliere e da confrontare con int orario[]
  DateTime time = rtc.now();
  // Serial.println(time.timestamp(DateTime::TIMESTAMP_TIME));
  if (time.hour() == o[0] && time.minute() == o[1]) 
    return true;
  return false;
}

void openServo () {
  servoDX.write(90); 
  servoSX.write(0); 
}

void closeServo () {
  servoSX.write(90); 
  servoDX.write(0); 
}

void eroga(){
  while (scale.get_units() < quantita){
    Serial.println(scale.get_units());
    openServo();
    delay(100);
  }
  closeServo();
}

int availableMemory() {
    // Use 1024 with ATmega168
    int size = 2048;
    byte *buf;
    while ((buf = (byte *) malloc(--size)) == NULL);
        free(buf);
    return size;
}

void loop() { 
  // Serial.println("Loop");
  //  DateTime currentTime = rtc.now();
   display.clearDisplay();
   display.setCursor(0, 0);
  int switchValue = digitalRead(switchPin);
  if (switchValue == HIGH) setupMode();
  else{
    char buffer[50];
    sprintf(buffer, " %02d:%02d:%02d\n", rtc.now().hour(), rtc.now().minute(), rtc.now().second());
    display.setCursor(centerDisplay(buffer), 4);
    display.println(buffer);
    strcpy(buffer, "Verranno erogati ");
    display.setCursor(centerDisplay(buffer), 18);
    display.println(buffer);
    sprintf(buffer, "%dg", quantita);
    display.setCursor(centerDisplay(buffer), 24);
    display.println(buffer);
    strcpy(buffer, " di cibo alle ");
    display.setCursor(centerDisplay(buffer), 32);
    display.println(buffer);
    sprintf(buffer, "%02d:%02d", orario[0], orario[1]);
    display.setCursor(centerDisplay(buffer), 40);
    display.println(buffer);
    display.display();
  }

int o[2] = {14, 00};
int peso = scale.get_units(10);
// Serial.println(availableMemory());
// availableMemory();
Serial.println(peso);
// if (checkTime(o)){
//   Serial.print("check: ");
//   Serial.println(checkTime(o));
//   eroga();
// }


delay(100);
}
