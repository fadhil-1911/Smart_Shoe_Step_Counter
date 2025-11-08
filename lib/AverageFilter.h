#ifndef AVERAGE_FILTER_H
#define AVERAGE_FILTER_H

#include <Arduino.h>

//#define FILTER_SIZE 20  // Boleh ubah ikut keperluan (di letak di ino sebelum #include "AverageFilter.h")

struct AverageFilter {
  float readings[FILTER_SIZE];  // Simpan bacaan
  int readIndex;              // Indeks semasa
  float total;                // Jumlah bacaan
  float average;              // Purata bacaan
};

// Inisialisasi filter
inline void initFilter(AverageFilter &filter) {
  filter.readIndex = 0;
  filter.total = 0;
  filter.average = 0;
  for (int i = 0; i < FILTER_SIZE; i++) {
    filter.readings[i] = 0;
  }
}

// Kemaskini filter dengan bacaan baru
inline void updateFilter(AverageFilter &filter, float newReading) {
  // Tolak bacaan lama
  filter.total -= filter.readings[filter.readIndex];

  // Masukkan bacaan baru
  filter.readings[filter.readIndex] = newReading;
  filter.total += newReading;

  // Gerak indeks
  filter.readIndex++;
  if (filter.readIndex >= FILTER_SIZE) {
    filter.readIndex = 0;
  }

  // Kira purata
  filter.average = filter.total / FILTER_SIZE;
}

// Dapatkan nilai purata
inline float getAverage(const AverageFilter &filter) {
  return filter.average;
}

#endif

