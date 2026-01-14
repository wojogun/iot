#ifndef MOD_LOGIC_STORM_H
#define MOD_LOGIC_STORM_H

#include <Arduino.h>
#include "mod_weather.h"

// Storm-Mode wie bisher (wird von mod_mqtt_house1 genutzt)
enum StormMode {
  STORM_MODE_AUTO,
  STORM_MODE_FORCED_ON,
  STORM_MODE_FORCED_OFF
};

enum StormLevel {
  LEVEL_NORMAL,
  LEVEL_WARNING,
  LEVEL_STORM
};

// Init + Loop
void initStormLogic();
void loopStormLogic(const WeatherData &w);

// Broadcast Storm (zentral)
void handleStormBroadcast(const String& payload);

// Externe Steuerung (für Dashboard cmd AUTO/ON/OFF)
void setStormModeAuto();
void setStormModeOn();
void setStormModeOff();

StormMode  getStormMode();
StormLevel getStormLevel();
bool       isStormActive();

// Für UI/Weather-Logic
bool isStormWarningActive();

// Zwingt UI/output/Display refresh für den aktuellen level 
void refreshStormUi();

#endif