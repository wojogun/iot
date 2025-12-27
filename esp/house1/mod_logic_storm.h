#ifndef MOD_LOGIC_STORM_H
#define MOD_LOGIC_STORM_H

#include <Arduino.h>
#include "mod_weather.h"

enum StormMode {
    STORM_MODE_AUTO,
    STORM_MODE_FORCED_ON,
    STORM_MODE_FORCED_OFF
};

enum StormState {
    STORM_STATE_NORMAL,
    STORM_STATE_STORM
};

// nur intern (für UI/Outputs)
enum StormLevel {       
    LEVEL_NORMAL, 
    LEVEL_WARNING, 
    LEVEL_STORM 
};

void initStormLogic();
void loopStormLogic(const WeatherData& w);

// externe Steuerung
void setStormModeAuto();
void setStormModeOn();
void setStormModeOff();

StormMode  getStormMode();
StormState getStormState();
StormLevel getStormLevel();
bool       isStormActive();

// Für Debug/UI
bool isStormWarningActive();

#endif