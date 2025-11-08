#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Arduino.h>
#include "AverageFilter.h"
//#include "Battery.h"
#include <U8g2lib.h>
#include "batteryCharging.h"
#include "BatteryEEPROM.h"


// Nyatakan rujukan kepada objek global
extern Battery myBattery;
extern AverageFilter voltFilter;
extern U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2;
extern StepCounter stepper;
extern Charger batteryCharger;
extern BatteryEEPROM batteryEEPROM;
extern float vbat;

// Nyatakan fungsi luar
long readVcc();

// Variable global OLED
extern unsigned long lastTimeOled;

//=== untuk animate battery charging ===
int animateCharging(bool isCharging) {
  static int animPercent = 0; // perlu static untuk global
  static unsigned long lastUpdate = 0; // perlu static untuk global
  unsigned long now = millis();

  if (isCharging) {
    if (now - lastUpdate > 0) {  // setiap 150ms naik 1%
      lastUpdate = now;

      if (animPercent < 100)
        animPercent+= 5;  // naik perlahan-lahan
      else
        animPercent = 0;  // 0 = berdenyut ulang.. 100 = naik mati di 100
    }
  } else {
    animPercent = 0;  // reset ke 0 bila tak cas
  }

  return animPercent;
}


//======== OLED FUNTION =======
void oledDisplay() {
  if (millis() - lastTimeOled >= 100) {
    lastTimeOled = millis();

    batteryCharger.update();
    bool isCharging = batteryCharger.charging();  // 

    float newReading = myBattery.readVoltage();  //readBatteryVoltage(BAT_PIN); vbat
    updateFilter(voltFilter, newReading);        // kemas kini filter
    vbat = getAverage(voltFilter);
    int batPercent = voltageToPercent(vbat);

    //=== State bateri charge untuk animasi bar ===
    int batValue;
    if (isCharging) {
      batValue = animateCharging(true);  // animasi naik 0 → 100
    } else {
      animateCharging(false);  // reset ke 0
      batValue = batPercent;              // guna nilai sebenar bateri
    }                                     //-------------------------------------------


    u8g2.firstPage();
    do {

      // --- Ikon bateri (20x8) ---
      u8g2.drawFrame(2, 2, 21, 8);  // outline
      u8g2.drawBox(23, 4, 2, 4);    // kepala

      // --- Isi 6 bar ---
      //int numBars = map(batValue, 0, 100, 0, 6); // VERSI 4.20 > 100
      int numBars = round(batValue / (100.0 / 6.0));  // 16.6666 SETIAP BAR
      int barWidth = 2;                               // lebar setiap bar
      int gap = 1;                                    // jarak antara bar

      for (int i = 0; i < numBars; i++) {
        int x = 4 + i * (barWidth + gap);
        u8g2.drawBox(x, 4, barWidth, 4);
      }  //------------------------------

      /*/ --- % teks bat % ---
      char buf[10];                      // menyimpan teks yang nak dipaparkan.
      sprintf(buf, "%d%%", batPercent);  // %d = tempat letak untuk integer. %% = cara escape simbol % dalam sprintf.
      u8g2.setFont(u8g2_font_6x12_tr);
      u8g2.drawStr(26, 10, buf);  */

      // --- Step Count ---
      char buf2[12];
      sprintf(buf2, "%lu", stepper.stepCount);  // "%lu" = unsigned long integer”

      u8g2.setFont(u8g2_font_logisoso28_tf);
      int textWidth = u8g2.getStrWidth(buf2);
      int x = (128 - textWidth) / 2;
      int y = 45;
      u8g2.drawStr(x, y, buf2);

      // --- Label ---
      const char* label = "STEP COUNT";
      u8g2.setFont(u8g2_font_6x12_tr);
      u8g2.drawStr((128 - u8g2.getStrWidth(label)) / 2, 62, label);

      /*/ --- Bat Volt --- (u8g2.setFont guna yang atas @ lebel)
      char buffer[10];
      float value = vbat;
      dtostrf(value, 4, 2, buffer);  // fungsi arduino (float, min width, precision, buffer)
      strcat(buffer, "V");           // Fungsi strcat() digunakan untuk sambung teks (C-string). Ia tambah teks kedua ke hujung teks pertama.
      u8g2.drawStr(99, 62, buffer);

      // --- Internal VCC --- u8g2.setFont guna yang atas @ lebel)
      float Vcc = readVcc() / 1000.0;  // tukar ke volt
      char buffer2[10];
      float value2 = Vcc;
      dtostrf(value2, 4, 2, buffer2);
      u8g2.drawStr(0, 62, buffer2); */

      // 🚨 Notis bateri low
      if (batPercent <= 20) {
        const char* warning = "BATTERY LOW!";
        u8g2.drawStr((128 - u8g2.getStrWidth(warning)) / 1, 10, warning);
      }


      /*/ monitor charging state
      String warning2;  //  = batteryCharger.getState();  // "FULL", "CHARGING", "IDLE/NO CHARGE"
      if (batteryCharger.charging()) {
        warning2 = "CHARGING";
      } else if (batteryCharger.full()) {
       warning2 = "BATTERY FULL";
       }
      else {
      warning2 = "🔋 IDLE";
      u8g2.drawStr((128 - u8g2.getStrWidth(warning2.c_str())) / 1, 10, warning2.c_str()); */
    } while (u8g2.nextPage());
  }
}
#endif
