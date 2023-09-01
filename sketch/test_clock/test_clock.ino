/* Timestamp functions using a DS1307 RTC connected via I2C and Wire lib
**
** Useful for file name
**		` SD.open(time.timestamp()+".log", FILE_WRITE) `
**
**
** Created: 2015-06-01 by AxelTB
** Last Edit:
*/

#include "RTClib.h"

RTC_DS3231 rtc;

void setup() {
  Serial.begin(9600);

#ifndef ESP8266
  while (!Serial)
    ;  // wait for serial port to connect. Needed for native USB
#endif

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  // if (!rtc.isrunning()) {
  //   Serial.println("RTC is NOT running, let's set the time!");
  //   // When time needs to be set on a new device, or after a power loss, the
  //   // following line sets the RTC to the date & time this sketch was compiled
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //   // This line sets the RTC with an explicit date & time, for example to set
  //   // January 21, 2014 at 3am you would call:
  //   //  rtc.adjust(DateTime(2023, 8, 23, 7, 2, 0));
  // }

  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  // January 21, 2014 at 3am you would call:
  //  rtc.adjust(DateTime(2023, 8, 23, 7, 2, 0));
}

void loop() {
  DateTime time = rtc.now();
  DateTime x = DateTime(2023, 8, 23, 7, 16, 0);


  if (time >= x) {
    Serial.println("******************L'orario attuale corrisponde all'orario impostato.");
  }

  Serial.println(String("DateTime:ULL:\t") + x.timestamp(DateTime::TIMESTAMP_FULL));

  //Full Timestamp
  Serial.println(String("DateTime::TIMESTAMP_FULL:\t") + time.timestamp(DateTime::TIMESTAMP_FULL));

  //Date Only
  Serial.println(String("DateTime::TIMESTAMP_DATE:\t") + time.timestamp(DateTime::TIMESTAMP_DATE));

  //Full Timestamp
  Serial.println(String("DateTime::TIMESTAMP_TIME:\t") + time.timestamp(DateTime::TIMESTAMP_TIME));

  Serial.println("\n");

  //Delay 5s
  delay(5000);
}
