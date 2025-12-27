#ifndef MOD_SENSOR_STEAM_H
#define MOD_SENSOR_STEAM_H

#include <Arduino.h>

struct SteamData { int raw; };

void initSteam();
SteamData readSteam();

// simulation
void setSteamSimulationEnabled(bool on);
void setSimulatedSteam(int raw);

#endif