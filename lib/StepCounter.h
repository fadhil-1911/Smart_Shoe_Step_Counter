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
