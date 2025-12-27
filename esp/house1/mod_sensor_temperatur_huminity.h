#ifndef MOD_SENSOR_TEMP_HUM_H
#define MOD_SENSOR_TEMP_HUM_H

#include <Arduino.h>

struct TempHumData {
    float temp;
    float hum;
};

void initTempHum();
TempHumData readTempHum();


// simulation
void setTempHumSimulationEnabled(bool on);
void setSimulatedTemp(float t);
void setSimulatedHum(float h);

#endif