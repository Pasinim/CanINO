// Librerie per il display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <HX711.h>
#include <RTClib.h>
#include <Servo.h>
Servo servoDX;
Servo servoSX;
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
  char str[50] = "SETUP";
  display.setCursor(centerDisplay(str), 0);
  display.println(str);
  sprintf(str, "ORARIO");
  display.setCursor(centerDisplay(str), 8);
  display.println(str);
  sprintf(str, "\nImposta l'orario di erogazione: %02d:%02d\n", orario[0], orario[1]);

  /** Definisco la variabile orarioErogazione: yyyy/mm/gg, orario[0]:orario[1]:00;  **/
  DateTime orarioErogazione(rtc.now().year(), rtc.now().month(), rtc.now().day(), orario[0], orario[1], 0);
  display.println(str);
  display.display();
}


/** Permette di impostare la quantità (g) di peso da erogare tramite il potenziometro.
 Il range di valori che assume va da `lower` a `upper`. 
 */
void weightSetup(){
  Serial.println("weight setup");
  int valorePotenziometro = analogRead(potPin);
  quantita = map(valorePotenziometro, 0, 1023, lower, upper);

  char str[50] = "SETUP";
  display.setCursor(centerDisplay(str), 0);
  display.println(str);
  sprintf(str, "QUANTITA");
  display.setCursor(centerDisplay(str), 8);
  display.println(str);
  sprintf(str, "\nImposta la quantita di cibo: %d", quantita);
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
* Restiutuisce true se l'orario
**/
bool checkTime(DateTime orarioErogazione){
  DateTime time = rtc.now();
  Serial.println(time.timestamp(DateTime::TIMESTAMP_TIME));
  if (time.hour() == orarioErogazione.hour() && time.minute() == orarioErogazione.minute()) 
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

void loop() { 
  Serial.println("Loop");
  //  DateTime currentTime = rtc.now();
   display.clearDisplay();
   display.setCursor(0, 0);
  int switchValue = digitalRead(switchPin);
  if (switchValue == HIGH) setupMode();
  else{
    openServo();
    delay(1000);
    closeServo();
    delay(1000);
  }
  //  else {
  //   // char buffer[50];
  //   // sprintf(buffer, "%02d:%02d:%02d\n", currentTime.hour(), currentTime.minute(), currentTime.second());
  //   // display.setCursor(centerDisplay(buffer), 0);
  //   // display.println(buffer);
  //   // buffer[0] = 0;
  //   // sprintf(buffer, "Verranno erogati %dg di cibo alle %d:%d\n", quantita, orario[0], orario[1]);
  //   // display.println(buffer);
  //   display.display();
  // }
display.display();
}
