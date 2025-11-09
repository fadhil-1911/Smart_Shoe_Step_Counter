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
        if (now - lastStepTime >= MIN_REFRACTORY) {  // JIKA SYARAT MASA MIN_REFRACTORY DI PENUHI
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
    
    
    
    
-------------------------------------------


  Step Timing Logic: PRE_REFRACTORY vs MIN_REFRACTORY

In impact-based step counting systems (using accelerometer or IMU), two timing parameters are required to ensure accurate step recognition and to prevent false counts:
	1.	PRE_REFRACTORY — validation time after an impact is detected
	2.	MIN_REFRACTORY — minimum time spacing between valid steps

These two parameters work together to eliminate noise, vibration bursts, foot bounce, and non-step foot movement.

⸻

1. PRE_REFRACTORY — Signal Validation Delay

“Wait before confirming the step.”

When an impact is detected, the system does not immediately increment the step counter.
Instead, it waits for a short duration (typically ~120–200 ms) to confirm that the signal stabilizes and that the impact truly represents a foot strike.

Purpose:

Attribute	Description
Function	Validate the impact before it is recognized as a step
Goal	Accuracy — avoid noise / micro-vibration being counted
Timing	Happens after impact detection, but before step confirmation
Why it matters	Impact signals often oscillate (sensor vibration / shoe bounce) and may trigger false positives

Analogy: Like double-checking the impact before deciding it’s a real step.

⸻

2. MIN_REFRACTORY — Minimum Step Spacing

“Ensure two steps are not counted too close together.”

Once a step has been confirmed, a cooldown period (typically ~300–500 ms) is enforced before the next step can be counted.
This prevents two rapid consecutive impacts from being recognized as multiple steps.

Purpose:

Attribute	Description
Function	Enforces a minimum time gap between steps
Goal	Biomechanics compliance — humans cannot step faster than a physical limit
Timing	Happens after a step has been counted
Why it matters	Prevents double-counting due to bouncing or left/right foot impacts being too close

Analogy: Like enforcing a minimum distance between consecutive steps.

⸻

  Side-by-Side Comparison

Parameter	When it is used	Primary Purpose	Effect
PRE_REFRACTORY	After impact is detected, before confirming a step	Ensure the impact is legitimate	Prevents noise / vibration from being counted
MIN_REFRACTORY	After a step has been counted	Ensure steps are not too close	Prevents double-counting (foot bouncing / cross-foot impacts)


⸻

  Example Step Cycle Timeline

Impact detected  →  [ PRE_REFRACTORY ]  →  Step confirmed  →  [ MIN_REFRACTORY ]  →  Next step
      0 ms             120–200 ms               STEP              300–500 ms

	•	PRE_REFRACTORY = validation delay
	•	MIN_REFRACTORY = cooldown delay between steps

⸻

  Parameter	Meaning
PRE = confirm step	(Is this impact valid?)
MIN = spacing between steps	(Don’t allow steps too close together)

*/





