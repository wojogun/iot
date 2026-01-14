#ifndef MOD_SENSOR_FAN_H
#define MOD_SENSOR_FAN_H

#include <Arduino.h>

void initFan();
void setFanPercent(int pct);     // 0..100
int  getFanPercent();

#endif