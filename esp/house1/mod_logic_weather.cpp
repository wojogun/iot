#include <Arduino.h>
#include "config.h"
#include "hardware.h"
#include "mod_config_mqtt.h"
#include "mod_logic_common.h"
#include "mod_logic_storm.h"
#include "mod_logic_weather.h"
#include "mod_weather.h"
#include "mod_sensor_lcd.h"
#include "mod_sensor_fan.h"

// ===== settings =====
static const unsigned long WEATHER_READ_MS = 1000;  // Sensor read interval
static const unsigned long MQTT_PUBLISH_MS = 1000;  // publish interval
static const unsigned long SCREEN_SWITCH_MS = 3000; // only in NORMAL

// ===== cached weather =====
static WeatherData cachedW{};
static bool hasWeather = false;
static unsigned long lastReadMs = 0;
static unsigned long lastPublishMs = 0;

// display rotate (nur NORMAL)
static unsigned long lastScreenSwitchMs = 0;
static bool showValuesScreen = false;

// helper: normal screens
static void showNormalTextScreen()
{
  printLcd("Haus 1", "Wetter normal", false);
}

static void showNormalValuesScreen(const WeatherData &w)
{
  String l1 = "TE:" + String(w.temp, 1) + " - LUFE:" + String((int)w.hum) + "%";
  String l2 = "RE:" + String(w.steam) + "% - WI:" + String(w.wind);
  printLcd(l1, l2, false);
}

// Wetter Temperatur - Farbe RGB Led
static bool tempLedActive = false;
static int tempColorState = 0; // 0=normal, 1=cold, 2=hot

void initWeatherLogic()
{
  lastReadMs = 0;
  lastPublishMs = 0;
  lastScreenSwitchMs = 0;
  showValuesScreen = false;
  hasWeather = false;
  showNormalTextScreen();
}

void loopWeatherLogic()
{
  const unsigned long now = millis();


  // Read sensors
  if (!hasWeather || (now - lastReadMs >= WEATHER_READ_MS))
  {
    lastReadMs = now;
    cachedW = readWeather();
    hasWeather = true;
  }

  // Sturm Logik verwendet cached Werte
  loopStormLogic(cachedW);

  // Publish cached values only every MQTT_PUBLISH_MS
  if (now - lastPublishMs >= MQTT_PUBLISH_MS)
  {
    lastPublishMs = now;

    publishMqtt(TOPIC_STATUS_TEMP, String(cachedW.temp, 1), true);
    publishMqtt(TOPIC_STATUS_HUM, String(cachedW.hum, 0), true);
    publishMqtt(TOPIC_STATUS_STEAM, String(cachedW.steam), true);
    publishMqtt(TOPIC_STATUS_WIND, String(cachedW.wind), true);
  }

  // LCD rotation only when NORMAL (no warning / no storm)
    if (!isStormActive() && !isStormWarningActive() && getGasStatus() == OFF && getPartyStatus() == OFF)
  {
    if (lastScreenSwitchMs == 0)
    {
      lastScreenSwitchMs = now;
      showValuesScreen = false;
      showNormalTextScreen();
    }

    if (now - lastScreenSwitchMs >= SCREEN_SWITCH_MS)
    {
      lastScreenSwitchMs = now;
      showValuesScreen = !showValuesScreen;

      if (showValuesScreen)
        showNormalValuesScreen(cachedW);
      else
        showNormalTextScreen();
    }
  }
  else
  {
    // stop normal rotation in warning/storm
    lastScreenSwitchMs = 0;
  }

  // ---------- TEMPERATUR RGB (Gas hat Vorrang) ---------
  if (getGasStatus() == ON)
  {
    tempColorState = 0;
    tempLedActive = false;
    return;
  }

  int newState = 0;
  if (cachedW.temp <= 5.0f)
    newState = 1;
  else if (cachedW.temp >= 30.0f)
    newState = 2;

  if (newState != tempColorState)
  {
    tempColorState = newState;

    if (newState == 1)
    {
      rgbSet(0, 0, 255, 150);
      tempLedActive = true;
    }
    else if (newState == 2)
    {
      rgbSet(255, 80, 0, 180);
      tempLedActive = true;
    }
    else
    {
      if (tempLedActive)
      {
        rgbOff();
        tempLedActive = false;
      }
    }
  }
}
