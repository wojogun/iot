#ifndef MOD_SENSOR_STEAM_H
#define MOD_SENSOR_STEAM_H

#include <Arduino.h>

struct SteamData { int raw; };              // 0 bis 4095

void initSteam();

// simulation
void setSteamSimulationEnabled(bool on);
void setSimulatedSteam(int pct);
// void setSimulatedSteam(int raw);         // Für Analoge werte 0-4095

SteamData readSteam();

int getSteamPercent();

#endif