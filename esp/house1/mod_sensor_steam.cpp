#include "hardware.h"
#include "mod_sensor_steam.h"

static bool simEnabled = false;
static int  simSteam = 0;

void initSteam() {
  pinMode(PIN_STEAM, INPUT);
}

void setSteamSimulationEnabled(bool on) { simEnabled = on; }

void setSimulatedSteam(int raw) {
  if (raw < 0) raw = 0;
  if (raw > 4095) raw = 4095;
  simSteam = raw;
}

SteamData readSteam() {
  SteamData d;
  d.raw = simEnabled ? simSteam : analogRead(PIN_STEAM);
  return d;
}