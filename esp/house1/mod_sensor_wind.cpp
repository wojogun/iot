// NOTIZ - Es gibt KEIN physische Windsensor im Haus 1. 
// Wind ist immer ein "demo" (0..100) wert von:
// - BTN2 click (wenn simulation is disabled!!)
// - MQTT simulation slider (wenn simulation is enabled in nodeRed )
//   (Topic: resort/house1/simulation/enabled)
// Aktuelle wind wert steuert die FAN proportioniert

#include "mod_sensor_wind.h"
#include "mod_sensor_fan.h"

static bool simEnabled = false;
static int currentWindPct = 0;   // single source of truth for storm logic

static inline int clampInt(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static int windToFanPercent(int windPercent) {
  windPercent = clampInt(windPercent, 0, 100);
  if (windPercent > 0 && windPercent < 20) return 20;
  return windPercent;
}

static void applyWind(int level) {
  level = clampInt(level, 0, 100);
  if (level == currentWindPct) return;

  currentWindPct = level;
  setFanPercent(windToFanPercent(currentWindPct));
}

void initWind() {
  simEnabled = false;
  currentWindPct = 0;
  setFanPercent(0);
}

void setWindSimulationEnabled(bool on) {
  simEnabled = on;

  if (!simEnabled) {
    applyWind(0);
  }
}

bool isWindSimulationEnabled() {
  return simEnabled;
}

void setSimulatedWind(int level) {
  if (!simEnabled) return;        // ignoriert/sperrt sleider inputs wenn simulation disabled ist.
  applyWind(level);
}

void setLocalWindPercent(int level) {
  if (simEnabled) return;         // ignoriert lokale Aktionen wenn simulation enabled ist
  applyWind(level);
}

int getCurrentWindPercent() {
  return currentWindPct;
}

WindData readWind() {
  WindData d;
  d.level = currentWindPct;
  return d;
}