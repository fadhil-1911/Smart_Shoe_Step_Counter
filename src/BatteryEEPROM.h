#ifndef BATTERY_EEPROM_H
#define BATTERY_EEPROM_H

#include <EEPROM.h>

struct BatteryEEPROM {
  const int ADDR_VBAT = 0;  // alamat EEPROM untuk simpan voltan
  float lastSavedVbat = 0.0;
  unsigned long lastSample = 0;

  void saveVbatToEEPROM(float vbat) {
    if (abs(vbat - lastSavedVbat) > 0.05) {  // hanya jika beza >0.05V
      EEPROM.put(ADDR_VBAT, vbat);
#if defined(ESP8266) || defined(ESP32)
      EEPROM.commit();  // wajib untuk ESP
#endif
      lastSavedVbat = vbat;
    }
  }

  float loadVbatFromEEPROM() {
    float v;
    EEPROM.get(ADDR_VBAT, v);
    if (isnan(v) || v < 2.0 || v > 5.0) v = 3.7;  // default fallback
    lastSavedVbat = v;
    return v;
  }

  // ✅ versi betul: terima nilai bacaan sebagai parameter
  void update(float vbat) {
    if (millis() - lastSample >= 2000) {
      lastSample = millis();
      saveVbatToEEPROM(vbat);
    }
  }
}; 

#endif
