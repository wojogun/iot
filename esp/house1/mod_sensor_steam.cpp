// #include "hardware.h"
// #include "mod_sensor_steam.h"

// static bool simEnabled = false;
// static int  simSteam = 0;

// static const int STEAM_DRY_RAW = 0;
// static const int STEAM_WET_RAW = 4095;

// void initSteam() {
//   pinMode(PIN_STEAM, INPUT);
// }

// void setSteamSimulationEnabled(bool on) { simEnabled = on; }

// void setSimulatedSteam(int raw) {
//   if (raw < 0) raw = 0;
//   if (raw > 4095) raw = 4095;
//   simSteam = raw;
// }

// SteamData readSteam() {
//   SteamData d;
//   d.raw = simEnabled ? simSteam : analogRead(PIN_STEAM);
//   return d;
// }

// int getSteamPercent() {
//  SteamData d = readSteam();
//   int raw = d.raw;

//   int pct = map(raw, STEAM_DRY_RAW, STEAM_WET_RAW, 0, 100);
//   return constrain(pct, 0, 100);
// }

#include "hardware.h"
#include "mod_sensor_steam.h"

// Simulation
static bool simEnabled = false;
static int  simSteamPct = 0;  // 0..100 (%)

// Kalibrierung/Orientierung:
// Du willst: trocken => 0%  | nass => 100%
// Wenn dein Sensor invertiert ist, tausche die Werte!
static const int STEAM_DRY_RAW = 0;      // trocken
static const int STEAM_WET_RAW = 4095;   // nass

void initSteam() {
  pinMode(PIN_STEAM, INPUT);
}

void setSteamSimulationEnabled(bool on) { 
  simEnabled = on; 
}

// Node-RED Slider liefert 0..100 (%)
void setSimulatedSteam(int pct) {
  pct = constrain(pct, 0, 100);
  simSteamPct = pct;
}

SteamData readSteam() {
  SteamData d;

  if (simEnabled) {
    // Simulation % -> raw (0..4095), damit alles konsistent bleibt
    d.raw = map(simSteamPct, 0, 100, STEAM_DRY_RAW, STEAM_WET_RAW);
    d.raw = constrain(d.raw, 0, 4095);
  } else {
    d.raw = analogRead(PIN_STEAM);
    d.raw = constrain(d.raw, 0, 4095);
  }

  return d;
}

int getSteamPercent() {
  SteamData d = readSteam();
  int raw = d.raw;

  int pct = map(raw, STEAM_DRY_RAW, STEAM_WET_RAW, 0, 100);
  pct = constrain(pct, 0, 100);
  return pct;
}
