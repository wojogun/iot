#include <Arduino.h>
#include "mod_logic_storm.h"
#include "config.h"
#include "hardware.h"
#include "mod_config_mqtt.h"
#include "mod_sensor_fan.h" 
#include "mod_sensor_lcd.h"
#include "mod_sensor_steam.h"
#include "mod_sensor_wind.h"
#include "mod_logic_common.h"

// ===================== LIMITS =====================
// ANALOG
// static const int STEAM_ON_LIMIT = 2500;
// static const int STEAM_OFF_LIMIT = 2300;

// PROZENT
static const int STEAM_ON_LIMIT = 60;
static const int STEAM_OFF_LIMIT = 55;

static const int WIND_ON_LIMIT = 80;
static const int WIND_OFF_LIMIT = 60;

static const unsigned long CLEAR_DELAY_MS = 3000;

// ===================== STATE =====================
static StormMode stormMode = STORM_MODE_AUTO;
static StormLevel level = LEVEL_NORMAL;

static bool steamHigh = false;
static bool windHigh = false;

static unsigned long clearSinceMs = 0;

// letzte Wetter Snapshots für Re-render LCD
static WeatherData lastW{};
static bool hasLastW = false;


// ---------- HELPERS ----------
static bool isOnPayload(const String &payload)
{
  String p = payload;
  p.trim();
  p.toUpperCase();
  return (p == "ON" || p == "1" || p == "TRUE" || p == "YES" || p == "PARTY");
}

static String weatherLine(const WeatherData &w)
{
  return "RE:" + String(w.steam) + "%  - WI:" + String(w.wind);
}

// Render helpers 
static void renderNormalUi()
{
  // nur öffnen, wenn keine Party aktiv
  if (getPartyStatus() == OFF)
  {
    ctrDoor(DOOR_OPEN);
    ctrWindow(WINDOW_OPEN);
  }

  // Effekte aus nur wenn kein Gas aktiv
  if (getGasStatus() == OFF)
  {
    switchLed(false);
    rgbOff();
    buzzer.playTone(0, 0);
  }

  printLcd("Haus 1: NORMAL", "", false);
}

static void renderWarningUi(const WeatherData &w)
{
  // WARNING: nix schließen, außer Party aktiv -> Party schließt Fenster
  if (getPartyStatus() == OFF)
  {
    ctrDoor(DOOR_OPEN);
    ctrWindow(WINDOW_OPEN);
  }

  // Haus 1: Bei Warnung Gelb leuchten dauerhaft
  switchLed(true);
  printLcd("Haus 1: WARNUNG", weatherLine(w), false);
}

static void renderStormUi(const WeatherData &w)
{
  printLcd("STURMWARNUNG!!", "Be Careful", false);
}

// Publish Haus-Status + Statusstorm + Broadcast
static void publishStatus()
{
  if (level == LEVEL_STORM)
  {
    publishMqtt(TOPIC_STATUS_HOUSE1, "STURM", true);
    publishMqtt(TOPIC_STATUS_STORM_HOUSE1, "ON", true);
    publishMqtt(TOPIC_BC_STORM, "ON", true);
  }
  else if (level == LEVEL_WARNING)
  {
    publishMqtt(TOPIC_STATUS_HOUSE1, "WARNING", true);
    publishMqtt(TOPIC_STATUS_STORM_HOUSE1, "OFF", true);
    publishMqtt(TOPIC_BC_STORM, "OFF", true);
  }
  else
  {
    publishMqtt(TOPIC_STATUS_HOUSE1, "NORMAL", true);
    publishMqtt(TOPIC_STATUS_STORM_HOUSE1, "OFF", true);
    publishMqtt(TOPIC_BC_STORM, "OFF", true);
  }
}

// ---------- STATUS NUR HAUS 1 !!! ----------
static void enterNormal()
{
  level = LEVEL_NORMAL; // Interne Status auf Normal setzen
  clearSinceMs = 0;
  publishStatus(); // Interne Status für UI immer aktualisieren (auch wenn Gas & Party aktiv sind)

  // UI nicht überschreiben, wenn Gas/Party aktiv
  if (getGasStatus() == ON || getPartyStatus() == ON)
  {
    return;
  }

  // NORMAL UI und Aktoren / Reaktion
  switchLed(false);
  rgbOff();
  setFanPercent(getCurrentWindPercent());
  buzzer.playTone(0, 0);
  ctrWindow(WINDOW_OPEN);
  ctrDoor(DOOR_OPEN);

  // NORMAL: LCD zurück auf Wetter - Normal
  printLcd("Haus 1", "Wetter NORMAL", false);
}

static void enterWarning(const WeatherData &w)
{
  level = LEVEL_WARNING; // Interne Status auf Warnung setzen
  clearSinceMs = 0;
  publishStatus(); // Interne Status für UI immer aktualisieren (auch wenn Gas & Party aktiv sind)

  if (getGasStatus() == ON)
  {
    printLcd("WARNUNG-WIND/REG", "& GASWARNUNG", false);
    return;
  }

   if (getPartyStatus() == ON)
  {
    printLcd("WARNUNG-WIND/REG", "PARTY STOPS SOON", false);
    return;
  }

  // NORMAL UI und Aktoren / Reaktion
  switchLed(true); // Warning: Gelbes Led an (nicht blinken)
  setFanPercent(getCurrentWindPercent());
  ctrWindow(WINDOW_OPEN);
  ctrDoor(DOOR_OPEN);

  // WARNUNG: LCD auf Warnung
  printLcd("WARNUNG-WIND/REG", weatherLine(w), false);
}

static void enterStorm(const WeatherData &w)
{
  level = LEVEL_STORM;
  clearSinceMs = 0;

  // STORM: immer schließen
  blinkLed();
  ctrDoor(DOOR_CLOSED);
  ctrWindow(WINDOW_CLOSED);
  if (getFanPercent() == 0)
  {
    setFanPercent(80);
  }

    if (getGasStatus() == ON)
  {
    printLcd("STURM WARNUNG", "& GASWARNUNG", false);
    return;
  }

   if (getPartyStatus() == ON)
  {
    printLcd("STURM WARNUNG", "NO PARTY TODAY:(", false);
    return;
  }
  renderStormUi(w);
  publishStatus(); // BC ON

  warnton();
}



// ===================== Broadcast Storm (Via Node-RED) =====================
void handleStormBroadcast(const String &payload)
{
  if (isOnPayload(payload))
  {
    // Broadcast erzwingt STURM (wie Forced ON)
    stormMode = STORM_MODE_FORCED_ON;
    if (level != LEVEL_STORM)
    {
      WeatherData dummy;
      dummy.steam = 70;
      dummy.wind = 80;
      enterStorm(dummy);
      printLcd("STURM", "BROADCAST", false);
    }
  }
  else
  {
    // Broadcast OFF -> zurück in AUTO (aber NICHT automatisch NORMAL erzwingen)
    stormMode = STORM_MODE_AUTO;
    // Level bleibt wie es ist – loopStormLogic entscheidet gleich sauber weiter.
    refreshStormUi();
  }
}

// ===================== Init / Loop =====================
void initStormLogic()
{
  publishMqtt(TOPIC_BC_STORM, "OFF", true); // Bei on boot auf false --> um "stale retained on" zu vermeiden
  publishStatus();                          // Initial publish damit UI sauber ist
}

void loopStormLogic(const WeatherData &w)
{
  const unsigned long now = millis();

  // letzte snapshot für refresh behalten
  lastW = w;
  hasLastW = true;

  // 1) Forced OFF: nur NORMAL/WARNING, niemals STURM + niemals BC ON
  if (stormMode == STORM_MODE_FORCED_OFF)
  {
    // Hysterese aktualisieren
    if (!steamHigh && w.steam >= STEAM_ON_LIMIT)
      steamHigh = true;
    if (steamHigh && w.steam <= STEAM_OFF_LIMIT)
      steamHigh = false;

    if (!windHigh && w.wind >= WIND_ON_LIMIT)
      windHigh = true;
    if (windHigh && w.wind <= WIND_OFF_LIMIT)
      windHigh = false;

    StormLevel newLevel = (steamHigh || windHigh) ? LEVEL_WARNING : LEVEL_NORMAL;

    if (newLevel == level)
      return;

    if (newLevel == LEVEL_WARNING)
      enterWarning(w);
    else
      enterNormal();

    return;
  }

  // 2) Forced ON: immer STURM
  if (stormMode == STORM_MODE_FORCED_ON)
  {
    if (level != LEVEL_STORM)
      enterStorm(w);
    return;
  }

  // 3) AUTO: NORMAL/WARNING/STURM mit Hysterese
  if (!steamHigh && w.steam >= STEAM_ON_LIMIT)
    steamHigh = true;
  if (steamHigh && w.steam <= STEAM_OFF_LIMIT)
    steamHigh = false;

  if (!windHigh && w.wind >= WIND_ON_LIMIT)
    windHigh = true;
  if (windHigh && w.wind <= WIND_OFF_LIMIT)
    windHigh = false;

  StormLevel target = LEVEL_NORMAL;
  if (steamHigh || windHigh)
    target = LEVEL_WARNING;
  if (steamHigh && windHigh)
    target = LEVEL_STORM;

  // Clear delay nur wenn wir gerade “zurück” gehen Richtung Normal
  if (target == LEVEL_NORMAL && level != LEVEL_NORMAL)
  {
    if (clearSinceMs == 0)
      clearSinceMs = now;
    if (now - clearSinceMs < CLEAR_DELAY_MS)
      return;
  }
  else
  {
    clearSinceMs = 0;
  }

  if (target == level)
    return;

  if (target == LEVEL_NORMAL)
    enterNormal();
  else if (target == LEVEL_WARNING)
    enterWarning(w);
  else
    enterStorm(w);
}

void refreshStormUi()
{
  // Wenn keine weather daten, verwende dummy daten
  WeatherData w = hasLastW ? lastW : WeatherData{};

  if (level == LEVEL_STORM)
  {
    renderStormUi(w);
  }
  else if (level == LEVEL_WARNING)
  {
    renderWarningUi(w);
  }
  else
  {
    renderNormalUi();
  }
}

// ===================== External API (Linker-Fix!) =====================
static WeatherData buildNowWeatherSnapshot()
{
  WeatherData w{};
  w.steam = getSteamPercent();         // 0..100
  w.wind  = getCurrentWindPercent();   // 0..100
  return w;
}

void setStormModeAuto()
{
  stormMode = STORM_MODE_AUTO;
  setFanPercent(0);
    // sofort UI aktualisieren mit aktuellen Sensor/Sim Werten
  WeatherData w = buildNowWeatherSnapshot();
  lastW = w;
  hasLastW = true;
  refreshStormUi();
}

void setStormModeOn()
{
  stormMode = STORM_MODE_FORCED_ON;
  setFanPercent(80);
   // sofort STORM UI mit aktuellen Werten anzeigen
  WeatherData w = buildNowWeatherSnapshot();
  lastW = w;
  hasLastW = true;
  enterStorm(w);        // zeigt direkt richtige Werte
}

void setStormModeOff()
{
  stormMode = STORM_MODE_FORCED_OFF;
  setFanPercent(0);
   // sofort UI aktualisieren
  WeatherData w = buildNowWeatherSnapshot();
  lastW = w;
  hasLastW = true;
  refreshStormUi();
}

StormMode getStormMode() { return stormMode; }
StormLevel getStormLevel() { return level; }

bool isStormActive() { return level == LEVEL_STORM; }
bool isStormWarningActive() { return level == LEVEL_WARNING; }
