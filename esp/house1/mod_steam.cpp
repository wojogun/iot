#include "mod_steam.h"
#include "hardware.h"


const int STEAM_WET_THRESHOLD = 2500;    // Hier definieren --> grober Startwert 

void initSteam() {
    pinMode(PIN_STEAM, INPUT);
}

SteamData readSteam() {
    SteamData data;
    data.rawData = analogRead(PIN_STEAM);
    return data;
}

bool isSteamWet() {
    return analogRead(PIN_STEAM) > STEAM_WET_THRESHOLD;
}
