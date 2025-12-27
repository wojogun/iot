#include "mod_sensor_wind.h"

static bool simEnabled = false;
static int simulatedWind = 0;

void initWind() {
  simEnabled = false;
  simulatedWind = 0;
}

void setWindSimulationEnabled(bool on) {
  simEnabled = on;
  if (!on) simulatedWind = 0;  // "echter" wind sensor existiert bei dir nicht -> dann 0
}

void setSimulatedWind(int level) {
  if (level < 0) level = 0;
  if (level > 100) level = 100;
  simulatedWind = level;
}

WindData readWind() {
  WindData d;
  d.level = simEnabled ? simulatedWind : 0;
  return d;
}