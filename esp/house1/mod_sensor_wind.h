#ifndef MOD_SENSOR_WIND_H
#define MOD_SENSOR_WIND_H

#include <Arduino.h>

struct WindData {
    int level;                      // 0..100 %
};

void initWind();
WindData readWind();

// Mode
void setWindSimulationEnabled(bool on);
bool isWindSimulationEnabled();

// Inputs
void setLocalWindPercent(int level);        // Für BTN2
void setSimulatedWind(int level);           // Für MQTT slider

// Current value
int getCurrentWindPercent();


#endif


// NOTIZ - Es gibt KEIN physische Windsensor im Haus 1. 
// Wind ist immer ein "demo" (0..100) wert von:
// - BTN2 click (wenn simulation is disabled!!)
// - MQTT simulation slider (wenn simulation is enabled in nodeRed )
//   (Topic: resort/house1/simulation/enabled)
// Aktuelle wind wert steuert die FAN proportioniert