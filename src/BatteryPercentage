#ifndef BATTERY_PERCENTAGE_H
#define BATTERY_PERCENTAGE_H


int voltageToPercent(float v) {
  if (v >= 4.20) return 100;
  else if (v >= 4.00) return 85 + (v - 4.00) * (15.0 / 0.20);
  else if (v >= 3.85) return 60 + (v - 3.85) * (25.0 / 0.15);
  else if (v >= 3.70) return 35 + (v - 3.70) * (25.0 / 0.15);
  else if (v >= 3.50) return 15 + (v - 3.50) * (20.0 / 0.20);
  else if (v >= 3.30) return (v - 3.30) * (15.0 / 0.20);
  else return 0;
}

#endif




/*
int voltageToPercent(float v) {
  if (v >= 4.13) return 100;
  else if (v >= 3.93) return 85 + (v - 3.93) * (15.0 / 0.20);
  else if (v >= 3.78) return 60 + (v - 3.78) * (25.0 / 0.15);
  else if (v >= 3.63) return 35 + (v - 3.63) * (25.0 / 0.15);
  else if (v >= 3.43) return 15 + (v - 3.43) * (20.0 / 0.20);
  else if (v >= 3.23) return (v - 3.23) * (15.0 / 0.20);
  else return 0;
} */



// Sesuai untuk Li-ion, LiFePO₄ (2.5–3.6V), atau NiMH (1.0–1.4V) — hanya ubah vMin & vMax.
/**
int voltageToPercent(float v, float vMin = 3.30, float vMax = 4.20) {
  if (v >= vMax) return 100;
  if (v <= vMin) return 0;
  float percent = ((v - vMin) / (vMax - vMin)) * 100.0;
  return (int)percent;
}

#endif */

/**
float smoothVoltage(float newVal, float oldVal, float alpha) {
  return oldVal + alpha * (newVal - oldVal);
} */



/**
enum BatteryType { LIPO_3V7, LIFEPO4_3V2, NIMH_1V2 };

int voltageToPercent(float v, BatteryType type = LIPO_3V7) {
  switch (type) {
    case LIPO_3V7:
      // guna fungsi asal Fadhil
      if (v >= 4.20) return 100;
      else if (v >= 4.00) return 85 + (v - 4.00) * (15.0 / 0.20);
      else if (v >= 3.85) return 60 + (v - 3.85) * (25.0 / 0.15);
      else if (v >= 3.70) return 35 + (v - 3.70) * (25.0 / 0.15);
      else if (v >= 3.50) return 15 + (v - 3.50) * (20.0 / 0.20);
      else if (v >= 3.30) return (v - 3.30) * (15.0 / 0.20);
      else return 0;

    case LIFEPO4_3V2:
      if (v >= 3.60) return 100;
      else if (v >= 3.30) return 90 + (v - 3.30) * (10.0 / 0.30);
      else if (v >= 3.20) return 60 + (v - 3.20) * (30.0 / 0.10);
      else if (v >= 3.10) return 30 + (v - 3.10) * (30.0 / 0.10);
      else if (v >= 2.90) return (v - 2.90) * (30.0 / 0.20);
      else return 0;

    case NIMH_1V2:
      if (v >= 1.40) return 100;
      else if (v >= 1.20) return 60 + (v - 1.20) * (40.0 / 0.20);
      else if (v >= 1.10) return 30 + (v - 1.10) * (30.0 / 0.10);
      else if (v >= 1.00) return (v - 1.00) * (30.0 / 0.10);
      else return 0;
  }
} */
