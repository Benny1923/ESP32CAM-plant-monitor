#ifndef BH1750_H
#define BH1750_H

#define BH1750_SENSOR_ADDR 0x23

    // Measurement at 1 lux resolution. Measurement time is approx 120ms.
#define CONTINUOUS_HIGH_RES_MODE 0x10
    // Measurement at 0.5 lux resolution. Measurement time is approx 120ms.
#define CONTINUOUS_HIGH_RES_MODE_2 0x11
    // Measurement at 4 lux resolution. Measurement time is approx 16ms.
#define CONTINUOUS_LOW_RES_MODE 0x13
    // Measurement at 1 lux resolution. Measurement time is approx 120ms.
#define ONE_TIME_HIGH_RES_MODE = 0x20
    // Measurement at 0.5 lux resolution. Measurement time is approx 120ms.
#define ONE_TIME_HIGH_RES_MODE_2 = 0x21
    // Measurement at 4 lux resolution. Measurement time is approx 16ms.
#define ONE_TIME_LOW_RES_MODE = 0x23

#endif