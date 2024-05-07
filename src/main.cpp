// Librerie per il display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <HX711.h>
#include <RTClib.h>
#include <Stepper.h>
// #include <Servo.h>

/** */  
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Creo un oggetto display. Vengono impostate le grandezze del display,
//  viene inizializzata la libreria per comunicare tramite il bus I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Step necessari per effettuare una rotazione completa del motore
const int stepsPerRevolution = 2048;
/** I pin del motore devono essere collegati rispettivamente ai pin IN1-IN3-IN2-IN4 */
Stepper myStepper = Stepper(stepsPerRevolution, 8, 10, 9, 11);

/** Inizializzazione dei pin */
const int potPin = A0; 
const int switchPin = 2;
const int buttonPin = 3;
const int data_loadCell = 6;
const int clock_loadCell = 7;

/** Range quantita di cibo erogabile, rispettivamente minimo e massimo */
const int lower = 150;
const int upper = 350;


/** Definizione delle variabili */
int orario[2]; /** orario nel formato hh:mm */
int quantita = lower; /** quantita di cibo da erogare [g] */ 
const int intervallo = 5; /** Passo con cui viene incrementato/decrementato l'orario (in minuti) */

int buttonValue = 0;      /** Segnale del pulsante */  
int lastButtonValue = 0; /** Ultimo segnale letto del pulsante*/  
int buttonPushCounter = 0;   /** Contatore che tiene traccia del numero di pressioni del tasto*/   


float calibration_factor = 1984.322021; /** Valore di calibrazione per la cella di carico*/ 
float offset_hx711 = 2321710; /** Offset della cella di carico */ 
HX711 scale; /** Variabile di istanza per utilizzare il modulo HX711*/


RTC_DS1307 rtc;  /** Variabile di istanza per utilizzare il modulo rtc*/



void setup() {                                          
  Serial.begin(9600); // Inizializza la comunicazione seriale a 9600 bps
  Wire.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // L'indirizzo I2C del display potrebbe essere diverso (0x3C è l'indirizzo più comune)
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  pinMode(switchPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  scale.begin(data_loadCell, clock_loadCell);
  scale.set_offset(offset_hx711); 
  scale.set_scale(calibration_factor);
  /** Imposta la tara*/
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
bool checkTime(){ //da togliere e da confrontare con int orario[]
  DateTime time = rtc.now();
  // Serial.println(time.timestamp(DateTime::TIMESTAMP_TIME));
  // if (time.hour() == o[0] && time.minute() == o[1]) 
  if (time.hour() == orario[0] && time.minute() == orario[1]) 
    return true;
  return false;
}

/**
 * Eroga il cibo fino a quando non si raggiunge la quantità impostata.
 * Il motore esegue un giro completo in senso orario, poi metà giro in senso orario.
 * Questo viene eseguito per evitare che il cibo rimanga incastrato nel tubo di erogazione.
 * 
 */
void eroga(){
  Serial.println("\t Eroga");
  while (scale.get_units(1) < quantita){
    Serial.print("peso attuale:"); 
    Serial.println(scale.get_units(1));
   	myStepper.setSpeed(15);
	  myStepper.step(stepsPerRevolution);
    myStepper.setSpeed(15);
	  myStepper.step(-stepsPerRevolution/3);
    delay(100);
  }
  return;
}

/**
 * Stampa sul display l'orario attuale e la quantità di cibo erogata.
 * @param currentTime orario corrente
 */
void printInfo(DateTime currentTime){
    char buffer[50];
/** Conversione dell'orario in stringhe*/
    char hourStr[3];
    char minuteStr[3];
    char secondStr[3];
    itoa(currentTime.hour(), hourStr, 10);
    itoa(currentTime.minute(), minuteStr, 10);
    itoa(currentTime.second(), secondStr, 10);

  /** Costruzione e stampa della stringa dell'orario */
    strcpy(buffer, " ");
    strcat(buffer, hourStr);
    strcat(buffer, ":");
    strcat(buffer, minuteStr);
    strcat(buffer, ":");
    strcat(buffer, secondStr);
    strcat(buffer, "\n");
    display.setCursor(centerDisplay(buffer), 4);
    display.println(buffer);

    /** Stampa la quantità di cibo erogta all'orario impostato*/
    strcpy(buffer, "Verranno erogati ");
    display.setCursor(centerDisplay(buffer), 18);
    display.println(buffer);
    itoa(quantita, buffer, 10);
    strcat(buffer, "g");
    display.setCursor(centerDisplay(buffer), 24);
    display.println(buffer);
    strcpy(buffer, " di cibo alle ");
    strcat(buffer, itoa(orario[0], hourStr, 10));
    strcat(buffer, ":");
    strcat(buffer, itoa(orario[1], minuteStr, 10));
    display.setCursor(centerDisplay(buffer), 32);
    display.println(buffer);

    display.display();
}

void debug() {
  Serial.println("********** DEBUG **********");
  Serial.print("orario: ");
  Serial.print(orario[0]);
  Serial.print(":");
  Serial.println(orario[1]);
  Serial.print("quantita: ");
  Serial.println(quantita);
  Serial.print("buttonPushCounter: ");
  Serial.println(buttonPushCounter);
Serial.print("ButtonValue: ");
  Serial.println(buttonValue);
  Serial.print("switchValue: ");
  Serial.println(digitalRead(switchPin));
  Serial.print("scale.get_units(1): ");
  Serial.println(scale.get_units(1));
  Serial.println("***************************");
  delay(500);
}

void loop() { 
  // debug();
   DateTime currentTime = rtc.now();
   display.clearDisplay();
   display.setCursor(0, 0);
  int switchValue = digitalRead(switchPin);
  // Serial.println(availableMemory());
  if (switchValue == HIGH) setupMode();
  else
    printInfo(currentTime);
  
  if (checkTime())
    eroga();
  
  
  //  Serial.print("peso attuale:"); 
  // Serial.println(scale.get_units(1));


// Serial.print(orario[0]);
// Serial.print(":");
// Serial.println(orario[1]);
  // eroga();
  // int o[2] = {currentTime.hour(), 42};
  // Serial.println(checkTime());

}
