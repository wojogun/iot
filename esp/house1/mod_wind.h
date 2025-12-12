#ifndef MOD_WIND_H
#define MOD_WIND_H

#include <Arduino.h>

struct WindData {
    int level;   // 0..100 (simuliert)
};

void initWind();
WindData readWind();

// für Simulation über MQTT / Node-RED
void setSimulatedWind(int level);

#endif