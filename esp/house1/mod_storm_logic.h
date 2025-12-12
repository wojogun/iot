#ifndef MOD_STORM_LOGIC_H
#define MOD_STORM_LOGIC_H

#include <Arduino.h>
#include "mod_weather.h"

// Modus -  wie wird Sturm "entschieden" / gemacht?
enum StormMode {
    STORMMODE_AUTO,      // Sensor-basiert
    STORMMODE_FORCE_ON,  // Node-RED zwingt Sturm EIN
    STORMMODE_FORCE_OFF  // Node-RED zwingt Sturm AUS
};

// Zustand - Normal oder Sturm?
enum StormState {
    STORMSTATE_NORMAL,
    STORMSTATE_STORM
};

extern StormMode  stormMode;
extern StormState stormState;

void initStormLogic();
void loopStormLogic();

// MQTT-Callback für dieses Haus (wird bei registerCallbackMqtt verwendet)
void handleStormMqtt(const String& topic, const String& payload);

#endif
