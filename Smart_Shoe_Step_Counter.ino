//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//               SMART SHOE STEP COUNTER
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Version: 0.9.5
// Release Category: Beta testing
// Release Date: 2025-11-08
// Author: fadhil1911
//
// Description:
//  Smart Shoe Step Counter using Arduino Nano, MPU6050,
//  OLED display, and battery monitoring system.
//
// Features:
//  - Step counting using MPU6050 sensor
//  - Real-time battery voltage & percentage display
//  - Charging animation with graphical bar
//  - Step reset button (digital input)
//  - EEPROM save/load for battery voltage
//  - AverageFilter to stabilize voltage readings
//  - OLED 128x64 display showing step count and battery status
//
// Hardware Connections:
//  - MPU6050: SDA=A4, SCL=A5
//  - Battery ADC: A0
//  - Reset Button: D10
//  - SmartTM1637 (optional): CLK=D2, DIO=D3
//  - OLED: I2C (SDA=A4, SCL=A5)
//
// Notes:
//  - Charging animation speed can be adjusted in animateCharging()
//  - Average filter size can be modified using FILTER_SIZE
//  - Tested on Arduino Nano/Uno
//  - SmartTM1637 display disabled by default (USE_TM1637=0)
//
// Changelog v0.9.5:
//  - fine tune for battery monitor animation bar
//  - fine tune for stepCounter parameter
//  - ready for release version
// 
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++


#include <Wire.h>
#include "MPU6050.h"  // SDA = A4, SCA = A5
#include "read_Sensor.h"
#include "StepCounter.h"
#include <U8g2lib.h>
#define FILTER_SIZE 40  // Boleh ubah ikut keperluan
#include "AverageFilter.h"
#include "batteryRead.h"
#include "BatteryPercentage.h"
#include "OledDisplay.h"
#include "batteryCharging.h"
#include "BatteryEEPROM.h"

//=== SmartTM1637 pinout ===
#define CLK_PIN 2     // Pin D2
#define DIO_PIN 3     // Pin D3
#define RESET_PIN 10  // Pin D10
#define BAT_PIN A0    // Battery input pin

#define USE_TM1637 0
#ifdef USE_TM1637
#include <SmartTM1637.h>
SmartTM1637 display(CLK_PIN, DIO_PIN);
#endif


//================== Object ===========================
SensorData sensor;       // Objek sensor
StepCounter stepper;  // Objek step
// U8G2_R0 = orientasi normal (boleh tukar R1/R2/R3 untuk rotate)
U8G2_SH1106_128X64_NONAME_2_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE);
AverageFilter voltFilter;    // MA FILTER
Battery myBattery(BAT_PIN);  // cipta objek bernama myBattery
Charger batteryCharger;      // "batteryCharging.h"
BatteryEEPROM batteryEEPROM;

//================= Global Variable ============================
bool lastState = HIGH;
unsigned long lastTimeLoop = 0;
unsigned long lastDebounceTime = 0;
unsigned long lastTimeVoltage = 0;
unsigned long lastTimeOled = 0;
float vbat = 0.0;  //  Definisi sebenar global vbat untuk guna pada eeprom setup


//================================================================================
//                                FUNTION
//================================================================================

//======== READ INTERNAL VOLTAGE =======
long readVcc() {
  // measure Vcc using internal 1.1V reference
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2);
  ADCSRA |= _BV(ADSC);
  while (bit_is_set(ADCSRA, ADSC))
    ;
  uint16_t result = ADCL;
  result |= ADCH << 8;
  // 1125300 = 1.1V * 1023 * 1000 (mV)
  long vcc = 1125300L / result;  // mV
  return vcc;                    // in mV
}

//======== RESET COUNTER FUNTION =======
void resetCount() {
  int reading = digitalRead(RESET_PIN);

  // Butang ditekan (LOW sebab INPUT_PULLUP)
  if (reading == LOW && lastState == HIGH && (millis() - lastDebounceTime > 80)) {
    stepper.stepCount = 0;  // Reset step
    //Serial.println("Step counter reset");
    lastDebounceTime = millis();
  }
  lastState = reading;
}

void debug_2() {
  Serial.print("State: ");
  Serial.print(batteryCharger.getState());
  Serial.print(" | charging: ");
  Serial.print(batteryCharger.charging());
  Serial.print(" | full: ");
  Serial.println(batteryCharger.full());
}

void debug_3() {
  //--- Internal voltage ---
  float Vcc = readVcc() / 1000.0;  // tukar ke volt
  Serial.print(" | VCC: ");
  Serial.print(Vcc);

  //--- Battery ADC ---
  int adc = analogRead(BAT_PIN);
  Serial.print(" | ADC: ");
  Serial.println(adc);
}

void primeFilterWithValue(AverageFilter& filter, float value) {
  filter.total = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    filter.readings[i] = value;
    filter.total += value;
  }
  filter.average = value;
  filter.readIndex = 0;
}


// --- Untuk init display ---
void oledInitDisplay() {

  u8g2.firstPage();
  do {
    const char* label = "WELCOME!";
    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.drawStr((128 - u8g2.getStrWidth(label)) / 2, 25, label);

    const char* label2 = "PEDOMETER v1.0.0";
    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.drawStr((128 - u8g2.getStrWidth(label2)) / 2, 35, label2);

  } while (u8g2.nextPage());
} 




//================================================================================
//                                VOID SETUP
//================================================================================
void setup() {
  Serial.begin(115200);
  Wire.begin();                      // Wire begin
  display.begin(7);                  // Initialize with max brightness SmartTM1637
  u8g2.begin();                      // Oled begin
  pinMode(RESET_PIN, INPUT_PULLUP);  // Reset button input. Use internal INPUT_PULLUP
  pinMode(BAT_PIN, INPUT);           // Battery pin input

  initFilter(voltFilter);                       // AverageFilter
  batteryCharger.begin(A1, A2);                 // batteryCharging
  batteryCharger.update();                      // batteryCharging
  bool isCharging = batteryCharger.charging();  // state


  vbat = batteryEEPROM.loadVbatFromEEPROM();  // Load vbat dari eeprom
  primeFilterWithValue(voltFilter, vbat);     // Untuk stabil bacaan semasa init
  oledInitDisplay();                   // Init display


  sensor.mpu.initialize();                         // Init MPU6050
  if (!sensor.mpu.testConnection()) {              // Check the sensor connection
    Serial.println("MPU6050 connection failed!");  // Print if connection failed
    while (1)                                      // Stop the program if connection failed!"
      ;
  }

  //=== Pre reading for MPU6050 ===
  int16_t ax, ay, az, gx, gy, gz;
  sensor.mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  sensor.lastAX = ax / 16384.0;
  sensor.lastAY = ay / 16384.0;
  sensor.lastAZ = az / 16384.0;

  delay(3000);
}

//================================================================================
//                                VOID LOOP
//================================================================================
void loop() {
  if (millis() - lastTimeLoop >= 05) {  // //20ms/50HZ
    lastTimeLoop = millis();

    //=== Baca sensor ====
    sensor.read();  // SensorData
    resetCount();   // reset count button
    stepper.impactStrength = sensor.impactStrength;
    //------------------------
    //=== Update counter ===
    stepper.update();  // ImpactDetector
    //====== SmartTM1637 ======
    display.printNumber(stepper.stepCount, false);

    //debug_2();  // battery state monitor
    //debug_3(); // internal & battery volt
  } 
  batteryCharger.update();
  batteryCharger.printStatus();
  oledDisplay(); 
  //--- Simpan ke EEPROM (auto setiap 2s jika beza >0.05V) ---
  batteryEEPROM.update(vbat);
  //Serial.println(vbat);
}
