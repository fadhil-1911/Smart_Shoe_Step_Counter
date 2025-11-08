#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>


struct Battery {
  int pin;           // pin analog A0
  float VREF;        // voltan rujukan 5V JIKA ARDUINO NANO/UNO
  float R1, R2;      // resistor divider R1 100 R2 100 (100000.0;)
  float CAL_FACTOR;  // faktor pembetulan 0.1

  // --- Fungsi untuk baca voltan ---
  // 🧠 Constructor — dijalankan bila objek dicipta
  Battery(
    int p,
    float vref = 5.0,
    float r1 = 100000.0,
    float r2 = 100000.0,
    float cal = 0.875)  // calibration (jika perlu)
    : pin(p), VREF(vref), R1(r1), R2(r2), CAL_FACTOR(cal) {}

  float readVoltage() {

    int adc = analogRead(pin);               // baca nilai analog (0–1023)
    float vAtPin = adc * (VREF / 1023.0);    // tukar kepada voltan sebenar di pin
    float vbat = vAtPin * ((R1 + R2) / R2);  // kira voltan bateri berdasarkan divider
    vbat *= CAL_FACTOR;
    return vbat;
  }
};

// 👉 Deklarasi global vbat (bukan definisi)
extern float vbat;

#endif

