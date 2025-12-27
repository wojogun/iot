#ifndef MOD_SENSOR_WIND_H
#define MOD_SENSOR_WIND_H

#include <Arduino.h>

struct WindData {
    int level;                      // 0..100
};

void initWind();
WindData readWind();

// simulation
void setWindSimulationEnabled(bool on);
void setSimulatedWind(int level);

#endif
