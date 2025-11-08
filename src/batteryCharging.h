// Battery charging state header

#ifndef BATTERY_CHARGING_H
#define BATTERY_CHARGING_H
//#include "Battery.h"
#include <Arduino.h>


//Battery myBattery_2(BAT_PIN);

struct Charger {
  int pinCHRG;
  int pinSTDBY;
  bool isCharging;
  bool isFull;

  // Inisialisasi pin
  void begin(int chrg, int stdby) {
    pinCHRG = chrg;
    pinSTDBY = stdby;
    pinMode(pinCHRG, INPUT_PULLUP);   // CHRG pin (LED merah)
    pinMode(pinSTDBY, INPUT_PULLUP);  // STDBY pin (LED biru/hijau)
  }

  // Kemas kini status semasa
  void update() {
    // Aktif rendah: LOW = LED ON
    bool chg = (digitalRead(pinCHRG) == LOW);
    bool full = (digitalRead(pinSTDBY) == LOW);

    //bool chg = (analogRead(pinCHRG) > 4.00);    // pin 6 (JIKA CAS pinCHRG 4.1v & pinSTDBY 0.6v / JIKA TAK CAS cas pinCHRG && pinSTDBY = 4.1v)
    //bool full = (analogRead(pinSTDBY) < 4.00);  // pin 7 

    isCharging = chg && !full;
    isFull = full && !chg;

  }

  // Status teks
  String getState() {
    if (isFull) return "FULL";
    else if (isCharging) return "CHARGING";
    else return "IDLE/NO CHARGE";
  }

  // Akses boolean (lebih tepat)
  bool charging() {
    return isCharging;
  }
  bool full() {
    return isFull;
  }


  // Papar maklumat ke Serial
  void printStatus() {
    Serial.print("STATE: ");
    Serial.print(getState());
    Serial.print(" | CHGpin: ");
    Serial.print(digitalRead(pinCHRG));
    Serial.print(" | FULLpin: ");
    Serial.println(digitalRead(pinSTDBY));
  }
};

#endif
