#include <Arduino.h>
#include "config.h"
#include "mod_logic_common.h"
#include "mod_config_mqtt.h"
#include "mod_logic_storm.h"
#include "mod_sensor_fan.h"
#include "mod_sensor_wind.h"
#include "mod_sensor_steam.h"
#include "mod_sensor_temperatur_huminity.h"

static bool subscribed = false;

static bool isOn(const String& s) {
  String x = s;
  x.trim();
  x.toUpperCase();
  return (x == "ON" || x == "1" || x == "TRUE" || x == "YES");
}

static void ensureSubscribed() {
  if (!isConnectedMqtt()) return;
  if (subscribed) return;

  // Commands / Simulation
  subscribeMqtt(TOPIC_STORM_CMD);
  subscribeMqtt(TOPIC_SIM_ENABLED);
  subscribeMqtt(TOPIC_SIM_WIND);
  subscribeMqtt(TOPIC_SIM_STEAM);
  subscribeMqtt(TOPIC_SIM_TEMP);
  subscribeMqtt(TOPIC_FAN_CMD);

  subscribed = true;
  Serial.println("[House1] MQTT subscribed");
}

void handleMqttHouse1(const String& topic, const String& payload) {
// static void onMqttMessage(const String& topic, const String& payload) {
  // --- STORM MODE ---
  if (topic == TOPIC_STORM_CMD) {
    String p = payload; p.trim(); p.toUpperCase();
    if (p == "AUTO") setStormModeAuto();
    else if (p == "ON") setStormModeOn();
    else if (p == "OFF") setStormModeOff();
    Serial.printf("[House1] storm/cmd = %s\n", p.c_str());
    return;
  }

  // --- GLOBAL SIM ENABLE ---
  if (topic == TOPIC_SIM_ENABLED) {
    bool on = isOn(payload);

    setTempHumSimulationEnabled(on);
    setSteamSimulationEnabled(on);
    setWindSimulationEnabled(on);
    // Wind ist bei dir sowieso simuliert – aber wir "resetten" bei OFF, damit wirklich OFF ist:
    //if (!on) setSimulatedWind(0);

    Serial.printf("[House1] simulation/enabled = %s\n", on ? "ON" : "OFF");
    return;
  }

  // --- SIM WIND ---
  if (topic == TOPIC_SIM_WIND) {
    int lvl = payload.toInt();
    setSimulatedWind(lvl);
    Serial.printf("[House1] sim/wind = %d\n", lvl);
    return;
  }

  // --- SIM STEAM ---
  if (topic == TOPIC_SIM_STEAM) {
    int raw = payload.toInt();
    setSimulatedSteam(raw);
    Serial.printf("[House1] sim/steam = %d\n", raw);
    return;
  }

  // --- SIM TEMP (TempHum Simulation muss ON sein, sonst wird’s zwar gesetzt, aber nicht genutzt) ---
  if (topic == TOPIC_SIM_TEMP) {
    float t = payload.toFloat();
    setSimulatedTemp(t);
    Serial.printf("[House1] sim/temp = %.2f\n", t);
    return;
  }

  if (topic == TOPIC_FAN_CMD) {
    int pct = payload.toInt();
    setFanPercent(pct);
    publishMqtt(TOPIC_STATUS_FAN, String(getFanPercent()), true);
    return;
  }
}

static void mqttDispatcher(const String& topic, const String& payload) {
  handleMqttCommon(topic, payload); // Gas/Party broadcast
  handleMqttHouse1(topic, payload); // Simulation/Commands
}

void initMqttHouse1() {
 subscribed = false;
  registerCallbackMqtt(mqttDispatcher);
  ensureSubscribed();
}

void loopMqttHouse1() {
  // falls reconnect passiert ist → erneut subscriben
  if (!isConnectedMqtt()) {
    subscribed = false;
    return;
  }
  ensureSubscribed();
}
