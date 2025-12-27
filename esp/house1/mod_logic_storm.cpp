#include <Arduino.h>
#include "config.h"
#include "hardware.h"
#include "mod_logic_storm.h"
#include "mod_config_mqtt.h"
#include "mod_sensor_lcd.h"

// ===================== LIMITS =====================
static const int STEAM_ON_LIMIT = 2500;
static const int STEAM_OFF_LIMIT = 2300;

static const int WIND_ON_LIMIT = 80;
static const int WIND_OFF_LIMIT = 60;

static const unsigned long CLEAR_DELAY_MS = 5000; // 5s bis wieder NORMAL

// ===================== INTERNAL =====================
static StormMode stormMode = STORM_MODE_AUTO;
static StormLevel level = LEVEL_NORMAL;

static unsigned long clearSinceMs = 0;

// ---------- helpers ----------
static String weatherLine(const WeatherData &w)
{
  return "R:" + String(w.steam) + " W:" + String(w.wind);
}

// static void publishStatus() {
//   if (level == LEVEL_STORM) {
//     publishMqtt(TOPIC_STATUS_HOUSE1, "STORM", true);
//     publishMqtt(TOPIC_STATUSSTORM_HOUSE1, (level == LEVEL_STORM) ? "ON" : "OFF", true);
//     publishMqtt(TOPIC_BC_STORM, "ON", true);
//   } else if (level == LEVEL_WARNING) {
//     publishMqtt(TOPIC_STATUS_HOUSE1, "WARNING", true); // optional: wenn du WARNING anzeigen willst
//     publishMqtt(TOPIC_BC_STORM, "OFF", true);
//   } else {
//     publishMqtt(TOPIC_STATUS_HOUSE1, "NORMAL", true);
//     publishMqtt(TOPIC_BC_STORM, "OFF", true);
//   }
// }

static void publishStatus()
{
  // Hausstatus (Text)
  if (level == LEVEL_STORM)
    publishMqtt(TOPIC_STATUS_HOUSE1, "STORM", true);
  else if (level == LEVEL_WARNING)
    publishMqtt(TOPIC_STATUS_HOUSE1, "WARNING", true);
  else
    publishMqtt(TOPIC_STATUS_HOUSE1, "NORMAL", true);

  // Storm-LED (Dashboard)
  publishMqtt(TOPIC_STATUSSTORM_HOUSE1, (level == LEVEL_STORM) ? "ON" : "OFF", true);

  // Broadcast nur wenn echter STORM aktiv
  publishMqtt(TOPIC_BC_STORM, (level == LEVEL_STORM) ? "ON" : "OFF", true);
}

static void enterNormal()
{
  level = LEVEL_NORMAL;
  clearSinceMs = 0;

  // Outputs normal
  switchLed(false);
  buzzer.playTone(0, 0);
  ctrDoor(DOOR_OPEN);
  ctrWindow(WINDOW_OPEN);
  ctrFan(FAN_OFF);

  // Display (Werte-Rotation machst du in WeatherLogic)
  printLcd("Haus 1", "Wetter normal", false);

  publishStatus();
}

static void enterWarning(const WeatherData &w)
{
  level = LEVEL_WARNING;
  clearSinceMs = 0;

  // WARNING: Tür/Fenster bleiben offen
  blinkLed();
  buzzer.playTone(0, 0);
  ctrFan(FAN_OFF);

  printLcd("Haus 1: WARNUNG", weatherLine(w), false);

  publishStatus();
}

static void enterStorm(const WeatherData &w)
{
  level = LEVEL_STORM;
  clearSinceMs = 0;

  // STORM: schließen + alarm + LED dauerhaft
  switchLed(true);
  ctrDoor(DOOR_CLOSED);
  ctrWindow(WINDOW_CLOSED);
  ctrFan(FAN_OFF);

  printLcd("Haus 1: STURM", weatherLine(w), false);
  publishStatus();

  // then buzzer (blocking, but UI already updated)
  warnton();
}

// ===================== API =====================
void initStormLogic()
{
  stormMode = STORM_MODE_AUTO;
  enterNormal();
}

void loopStormLogic(const WeatherData &w)
{
  const unsigned long now = millis();

  // -------- Forced modes --------
  if (stormMode == STORM_MODE_FORCED_ON)
  {
    if (level != LEVEL_STORM)
      enterStorm(w);
    else
      printLcd("Haus 1: STURM", weatherLine(w), false);
    return;
  }
  if (stormMode == STORM_MODE_FORCED_OFF)
  {
    if (level != LEVEL_NORMAL)
      enterNormal();
    return;
  }

  // -------- AUTO: conditions --------
  const bool steamOn = (w.steam >= STEAM_ON_LIMIT);
  const bool windOn = (w.wind >= WIND_ON_LIMIT);
  const bool stormCond = steamOn && windOn;
  const bool warnCond = steamOn || windOn;

  const bool clearCond = (w.steam < STEAM_OFF_LIMIT) && (w.wind < WIND_OFF_LIMIT);

  // 1) STORM wenn beide hoch
  if (stormCond)
  {
    if (level != LEVEL_STORM)
      enterStorm(w);
    else
      printLcd("Haus 1: STURM", weatherLine(w), false);
    return;
  }

  // 2) Wenn nicht mehr STORM: zurück auf WARNING (wenn noch 1 hoch) oder NORMAL (wenn alles unten)
  if (warnCond)
  {
    if (level != LEVEL_WARNING)
      enterWarning(w);
    else
      printLcd("Haus 1: WARNUNG", weatherLine(w), false);
    return;
  }

  // 3) Alles unten -> nach 5s NORMAL
  if (clearCond)
  {
    if (clearSinceMs == 0)
      clearSinceMs = now;
    if (now - clearSinceMs >= CLEAR_DELAY_MS)
    {
      if (level != LEVEL_NORMAL)
        enterNormal();
    }
  }
  else
  {
    clearSinceMs = 0;
  }
}

// ---------- external API ----------
void setStormModeAuto() { stormMode = STORM_MODE_AUTO; }
void setStormModeOn() { stormMode = STORM_MODE_FORCED_ON; }
void setStormModeOff() { stormMode = STORM_MODE_FORCED_OFF; }

StormMode getStormMode() { return stormMode; }
StormState getStormState() { return (level == LEVEL_STORM) ? STORM_STATE_STORM : STORM_STATE_NORMAL; }
bool isStormActive() { return level == LEVEL_STORM; }
bool isStormWarningActive() { return level == LEVEL_WARNING; }
StormLevel getStormLevel() { return level; }