#ifndef READ_GYRO_ACCEL_H
#define READ_GYRO_ACCEL_H

#include <Wire.h>
#include <MPU6050.h>

struct SensorData {
  MPU6050 mpu;

  // Bacaan terakhir
  float lastAX = 0, lastAY = 0, lastAZ = 0;

  // Delta untuk step detection
  float deltaX = 0, deltaY = 0, deltaZ = 0;

  // Output terkini
  float impactStrength = 0;  // m/s²
  float currentAcc = 0;      // m/s²

  void read() {
    int16_t axRaw, ayRaw, azRaw, gxRaw, gyRaw, gzRaw;
    mpu.getMotion6(&axRaw, &ayRaw, &azRaw, &gxRaw, &gyRaw, &gzRaw);

    // Tukar ke unit sebenar
    float ax = axRaw / 16384.0;  // g
    float ay = ayRaw / 16384.0;
    float az = azRaw / 16384.0;
    // float gx = gxRaw / 131.0;    // deg/s
    // float gy = gyRaw / 131.0;
    // float gz = gzRaw / 131.0;

    // 1. Magnitud perubahan accelerometer (impact strength)
    //float deltaAX = ax - lastAX;
    //float deltaAY = ay - lastAY;
    //float deltaAZ = az - lastAZ;

    deltaX = ax - lastAX;
    deltaY = ay - lastAY;
    deltaZ = az - lastAZ;

    //impactStrength = sqrt(deltaAX * deltaAX + deltaAY * deltaAY + deltaAZ * deltaAZ) * 9.81;  // m/s²
    impactStrength = sqrt(deltaX * deltaX + deltaY * deltaY + deltaZ * deltaZ) * 9.81; // m/s²

    // 2. Magnitud accelerometer semasa (untuk valley check)
    currentAcc = sqrt(ax * ax + ay * ay + az * az) * 9.81;  // m/s²

    // Simpan bacaan terakhir
    lastAX = ax;
    lastAY = ay;
    lastAZ = az;
  }
};

#endif
