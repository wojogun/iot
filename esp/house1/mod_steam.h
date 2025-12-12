#ifndef MOD_STEAM_H
#define MOD_STEAM_H

#include <Arduino.h>

struct SteamData {
    int rawData;   // 0..4095
};

extern const int STEAM_WET_THRESHOLD;  // // Schwellenwert nur DEKLARIEREN (extern)

void initSteam();
SteamData readSteam();
bool isSteamWet();   // helper für Logik

#endif