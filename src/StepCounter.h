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

  5. Step recognition (stepCount++)
     - After PRE_REFRACTORY completes,
       a step can be counted.
     - Must also satisfy MIN_REFRACTORY
       to ensure steps are not double-counted in a short period.

        if (now - lastStepTime >= MIN_REFRACTORY) {
            stepCount += 1 or 2;
            lastStepTime = now;
        }

  6. Reset state
     - impactDetected is reset so a new detection cycle can begin.

        impactDetected = false;

  ──────────────────────────────────────────────────────────
  FLOW SUMMARY
  ──────────────────────────────────────────────────────────
  Shoe impact → detect peak → wait minimum time → count step → reset
  ──────────────────────────────────────────────────────────

  Parameter tuning:
    PEAK_THRESHOLD       → minimum impact strength to detect a step
    MAX_IMPACT_THRESHOLD → prevent extremely large impacts (e.g., fall / noise spike)
    MIN_REFRACTORY       → minimum time between consecutive steps (anti double-count)
    PRE_REFRACTORY       → cooldown period after an impact is detected

*/


// Core logic in short
//  • Read acceleration.
//  • Detect a peak impact that exceeds the threshold.
//  • When a peak is detected → impactDetected = true.
//  • Wait cooldown time to avoid double counting.
//  • Count step when minimum time is satisfied.
//  • Reset, and repeat the process.

// What’s special about this logic?
//  • No valley detection needed (peak-only approach → suitable for shoe step detection).
//  • Two layers of anti-double-count:
//      • PRE_REFRACTORY (wait after impact)
//      • MIN_REFRACTORY (ensure steps are not too close together)
//
// This makes the system stable for:
// ✔ Slow walking
// ✔ Jogging
// ✔ Moderate impacts

If you want, I can also translate your ASCII flowchart into a clean English version so it matches this comment block perfectly. Do you want me to do that?

