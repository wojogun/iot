#ifndef MOD_TEMP_HUM_H
#define MOD_TEMP_HUM_H

#include <Arduino.h>

struct TempHumData {
    float temp;
    float hum;
};

void initTempHum();
TempHumData readTempHum();

#endif