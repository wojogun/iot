#include <Arduino.h>
#include "config.h"
#include "mod_config_mqtt.h"
#include "mod_logic_common.h"
#include "mod_logic_storm.h"
#include "mod_sensor_fan.h"
#include "mod_sensor_wind.h"
#include "mod_sensor_steam.h"
#include "mod_sensor_temperatur_huminity.h"

static bool subscribed = false;

static bool isOn(const String &s)
{
  String x = s;
  x.trim();
  x.toUpperCase();
  return (x == "ON" || x == "1" || x == "TRUE" || x == "YES" );
}

static void ensureSubscribed()
{
  if (!isConnectedMqtt())
    return;
  if (subscribed)
    return;

  // Broadcasts
  // subscribeMqtt(TOPIC_BC_STORM);     // Haus 1 löst BC Sturm aus
  subscribeMqtt(TOPIC_BC_GAS);
  subscribeMqtt(TOPIC_BC_PARTY);

  // Commands / Simulation
  subscribeMqtt(TOPIC_STORM_CMD);
  subscribeMqtt(TOPIC_SIM_ENABLED);
  subscribeMqtt(TOPIC_SIM_WIND);
  subscribeMqtt(TOPIC_SIM_STEAM);
  subscribeMqtt(TOPIC_SIM_TEMP);

  subscribed = true;
  Serial.println("[House1] MQTT subscribed (BC + CMD + SIM)");
}

void handleHouse1Topics(const String &topic, const String &payload)
{
  // --- STORM MODE ---
  if (topic == TOPIC_STORM_CMD)
  {
    String p = payload;
    p.trim();
    p.toUpperCase();
    if (p == "AUTO")
      setStormModeAuto();
    else if (p == "ON")
      setStormModeOn();
    else if (p == "OFF")
      setStormModeOff();
    Serial.printf("[House1] storm/cmd = %s\n", p.c_str());
        refreshStormUi();
    return;
  }

  // --- GLOBAL SIMULATION ENABLE ---
  if (topic == TOPIC_SIM_ENABLED)
  {
    bool on = isOn(payload);

    setTempHumSimulationEnabled(on);
    setSteamSimulationEnabled(on);
    setWindSimulationEnabled(on);

    Serial.printf("[House1] Simulation enabled = %s\n", on ? "ON" : "OFF");    // DEBUG
    return;
  }
  
  // --- SIMULATION INPUTS ---
  if (topic == TOPIC_SIM_WIND)
  {
    setSimulatedWind(payload.toInt());      //wird intern ignoriert, wenn Simulation disabled
    return;
  }
  if (topic == TOPIC_SIM_STEAM)
  {
    setSimulatedSteam(payload.toInt());
    return;
  }
  if (topic == TOPIC_SIM_TEMP)
  {
    setSimulatedTemp(payload.toFloat());
    return;
  }

}

static void mqttDispatcher(const String &topic, const String &payload)
{
  handleMqttCommon(topic, payload);
  handleHouse1Topics(topic, payload);

  Serial.printf("[MQTT] %s = %s\n", topic.c_str(), payload.c_str());
}

void initMqttHouse1()
{
  subscribed = false;
  registerCallbackMqtt(mqttDispatcher);
  ensureSubscribed();
}

void loopMqttHouse1()
{
  // falls reconnect passiert ist --> erneut subscriben
  if (!isConnectedMqtt())
  {
    subscribed = false;
    return;
  }
  ensureSubscribed();
}