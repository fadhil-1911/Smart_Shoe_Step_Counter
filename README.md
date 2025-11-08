# Smart_Shoe_Step_Counter

**Version:** 0.9.5 Beta
**Author:** fadhil1911
**Release Date:** 2025‑11‑08

Smart Shoe Step Counter with Arduino Nano, MPU6050, OLED display, and battery monitoring. Counts steps, displays battery status, and optionally shows step count on TM1637.

---

## Features

* Step counting using MPU6050 accelerometer
* Real-time battery voltage and percentage display
* Animated battery charging indicator
* Step reset button with debounce
* EEPROM save/load of battery voltage
* AverageFilter for stabilized voltage readings
* OLED 128x64 display with step count and battery status
* Optional SmartTM1637 numeric display (`USE_TM1637`)

**Note:** Thresholds can be tuned for different shoes or walking styles.

---

## Hardware Connections

| Component    | Connection           | Notes                                    |
| ------------ | -------------------- | ---------------------------------------- |
| MPU6050      | SDA=A4, SCL=A5       | I2C connection                           |
| Battery ADC  | A0                   | Through voltage divider R1=100k, R2=100k |
| Reset Button | D10                  | Active LOW, with debounce                |
| OLED Display | I2C (SDA=A4, SCL=A5) | 128x64 pixels                            |
| SmartTM1637  | CLK=D2, DIO=D3       | Optional, set `USE_TM1637=1`             |

**Battery voltage divider:** Connect battery(+) → R1 → A0, GND → R2 → junction with A0.

---

## Setup Instructions

1. Install required libraries:

   * `MPU6050` (I2C sensor)
   * `U8g2` (OLED display)
   * `SmartTM1637` (optional, only if `USE_TM1637=1`)
2. Connect hardware according to the table above.
3. Adjust `FILTER_SIZE` in the code to change smoothing for battery voltage.
4. If using TM1637 display, set `#define USE_TM1637 1` and configure CLK/DIO pins.
5. Upload the sketch to Arduino Nano/Uno.
6. Open Serial Monitor (115200 baud) to observe debug output.

---

## Usage

* Step count automatically updates based on foot impacts.
* Battery voltage, percentage, and charging status display on OLED.
* Press **Reset Button** (D10) to reset step counter.
* Battery voltage is saved to EEPROM every 2 seconds if the voltage changes >0.05V.
* OLED updates every 100ms; TM1637 (if enabled) updates every loop.

---

## Wiring Diagram

```
Battery (+) ---- R1 ---- A0 ---- R2 ---- GND
                   |
                  A0 (Arduino)
MPU6050 SDA ---- A4 (Arduino)
MPU6050 SCL ---- A5 (Arduino)
Reset Button ---- D10 (Arduino)
OLED SDA ---- A4 (Arduino)
OLED SCL ---- A5 (Arduino)
TM1637 CLK ---- D2 (Arduino) [Optional]
TM1637 DIO ---- D3 (Arduino) [Optional]
TP4056 CHRG ---- A2 (Arduino)
TP4056 STDBY --- A1 (Arduino)
```

---

## Thresholds & Customization

* `PEAK_THRESHOLD` and `MAX_IMPACT_THRESHOLD` in `StepCounter.h` can be tuned for walking style or shoe type.
* Charging animation speed is adjustable in `animateCharging()` function.
* Filter size for battery smoothing can be changed via `FILTER_SIZE`.

---

## Changelog

**v0.9.5 Beta** – 2025‑11‑08

* Initial stable release with step counter and battery monitoring
* Optimized OLED refresh and battery animation
* EEPROM voltage save every 2s if change >0.05V
* Debounced reset button implemented
* TM1637 optional (disabled by default)

---

## License

MIT License – Free to use, modify, and distribute.

---

If you like, I can also **make a GitHub-ready markdown with images for wiring** so you can drop it directly into your repo as `README.md`. This will include visual step connections and pinout diagram.

Do you want me to do that next?

