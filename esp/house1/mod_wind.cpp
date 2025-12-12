#include "mod_wind.h"


static int simulatedWind = 0;    // aktuell nur Simulation
// Später: MQTT-Callback → setSimulatedWind() → Node-RED-Slider = Windregler

void initWind() {
    simulatedWind = 0;
}

WindData readWind() {
    WindData data;
    data.level = simulatedWind;
    return data;
}

void setSimulatedWind(int level) {
    if (level < 0)   level = 0;
    if (level > 100) level = 100;
    simulatedWind = level;
}
