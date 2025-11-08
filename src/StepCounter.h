// Step counter logical header

#ifndef STEP_COUNTER_H
#define STEP_COUNTER_H

struct StepCounter {
  // === State & Counter ===
  float impactStrength = 0;  // m/s² (accel delta / magnitude deviation)
  float currentAcc = 0;      // m/s² (accel magnitude)
  unsigned long lastStepTime = 0;
  unsigned long lastImpactTime = 0;
  unsigned long stepCount = 0;
  bool impactDetected = false;  // simpan status peak jumpa ke belum

  // === Thresholds & Timing ===
  const float PEAK_THRESHOLD = 20.0;  // mesti lebih kuat dari ini.  20.
  const float MAX_IMPACT_THRESHOLD = 300.0;
  const unsigned long MIN_REFRACTORY = 350;  // ms elak double count
  const unsigned long PRE_REFRACTORY = MIN_REFRACTORY;

  // BERJALAN PASS: PEAK_THRESHOLD = 20.0; VALLEY_THRESHOLD = 0.7; MAX_IMPACT_THRESHOLD = 150.0; MIN_REFRACTORY = 350;
  // stable 1 = PEAK_THRESHOLD 20.0; VALLEY_THRESHOLD = 5.0; MAX_IMPACT_THRESHOLD = 200.0; MIN_REFRACTORY = 100;
  // stable 2 = PEAK_THRESHOLD 22.0; VALLEY_THRESHOLD = 5.0; MAX_IMPACT_THRESHOLD = 100.0; MIN_REFRACTORY = 300;
  // STABLE 3 = PEAK_THRESHOLD 20.0; VALLEY_THRESHOLD = 7.0; MAX_IMPACT_THRESHOLD = 150.0; MIN_REFRACTORY = 300; SLOW WALK (30 STEP) 96%, JOGGING 96%?

  // === Logik utama ===
  void update() {
    unsigned long now = millis();

    //  CARI IMPACT HENTAKAN KASUT.
    if (!impactDetected && impactStrength >= PEAK_THRESHOLD && impactStrength <= MAX_IMPACT_THRESHOLD) {
      impactDetected = true;  // JADIKAN impactDetected = true APABILA IMPACT HENTAKAN KASUT DI KESAN
      lastImpactTime = now;   // MULAKAN MASA UNTUK PRE_REFRACTORY
    }

    // KIRA LANGKAH JIKA impactDetected = true (ADA HENTAKAN KASUT)
    if (impactDetected) {                            // JIKA impactDetected = true
      if (now - lastImpactTime >= PRE_REFRACTORY) {  // JIKA SYARAT MASA PRE_REFRACTORY DI PENUHI
        if (now - lastStepTime >= MIN_REFRACTORY) {  // JIKA SYARAT MASA PRE_REFRACTORY DI PENUHI
          stepCount += 2;                            // HITUNG LANGKAH
          lastStepTime = now;                        // RESET UNTUK HITUNGAN SETERUSNYA
        }
        impactDetected = false;  // RESET UNTUK LANGKAH BARU
      }
    }
  }
};


#endif

/*
  ──────────────────────────────────────────────────────────
  STEP COUNTER LOGIC (MPU6050 / Accelerometer based)
  ──────────────────────────────────────────────────────────

  1. Sensor reads the acceleration magnitude → currentAcc
     - Typically calculated as: sqrt(x² + y² + z²)

  2. impactStrength is used to measure a peak impact
     - Usually derived from changes in acceleration (delta)
     - Example: |currentAcc - previousAcc|

  3. Peak detection (impactDetected = true):
     - If impactStrength exceeds PEAK_THRESHOLD
       and does not exceed MAX_IMPACT_THRESHOLD
         → the system detects a shoe impact
         → impactDetected = true
         → lastImpactTime is recorded

        if (!impactDetected &&
            impactStrength >= PEAK_THRESHOLD &&
            impactStrength <= MAX_IMPACT_THRESHOLD)
        {
            impactDetected = true;
            lastImpactTime = now;
        }

  4. Refractory timing (anti double-count)
     - After a single impact is detected,
       the system waits PRE_REFRACTORY (e.g., 350 ms)
       to prevent small shakes or vibrations from counting as another step.

        if (impactDetected && (now - lastImpactTime >= PRE_REFRACTORY)) {

  5. Step recognition (stepCount += 2)
     - After PRE_REFRACTORY completes,
       a step can be counted.
     - **We add 2 steps per detected impact**:
         • Assumes one detected peak represents one foot (e.g., right shoe)
         • Adds 1 step for the opposite foot (e.g., left shoe)
         → Simplifies counting while maintaining approximate step accuracy
     - Must also satisfy MIN_REFRACTORY
       to ensure steps are not double-counted in a short period.

        if (now - lastStepTime >= MIN_REFRACTORY) {
            stepCount += 2; // count both right and left steps
            lastStepTime = now;
        }

  6. Reset state
     - impactDetected is reset so a new detection cycle can begin.

        impactDetected = false;

  ──────────────────────────────────────────────────────────
  FLOW SUMMARY
  ──────────────────────────────────────────────────────────
  Shoe impact → detect peak → wait minimum time → count 2 steps → reset
  ──────────────────────────────────────────────────────────

  Parameter tuning:
    PEAK_THRESHOLD       → minimum impact strength to detect a step
    MAX_IMPACT_THRESHOLD → prevent extremely large impacts (e.g., fall / noise spike)
    MIN_REFRACTORY       → minimum time between consecutive steps (anti double-count)
    PRE_REFRACTORY       → cooldown period after an impact is detected

  Step count note:
    • One detected impact is assumed to represent one step per foot.
    • stepCount += 2 accounts for both right and left foot for simplicity.

*/





