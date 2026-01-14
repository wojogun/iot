#include <Arduino.h>
#include "mod_logic_common.h"
#include "config.h"
#include "mod_config_mqtt.h"
#include "hardware.h"
#include "mod_sensor_lcd.h"
#include "mod_logic_storm.h"

static Onoff currentGasStatus = OFF;
static Onoff currentPartyStatus = OFF;

static bool isOnPayload(const String &payload)
{
  String p = payload;
  p.trim();
  p.toUpperCase();
  return (p == "ON" || p == "1" || p == "TRUE" || p == "YES" || p == "PARTY");
}

static void publishGas()
{
  publishMqtt(TOPIC_STATUS_GAS_HOUSE1, currentGasStatus == ON ? "ON" : "OFF", true);
}

static void publishParty()
{
  publishMqtt(TOPIC_STATUS_PARTY_HOUSE1, currentPartyStatus == ON ? "PARTY" : "OFF", true);
}

void handleMqttCommon(const String &topic, const String &payload)
{
  Serial.print("[MQTT][COMMON] topic=");
  Serial.print(topic);
  Serial.print(" payload=");
  Serial.println(payload);
  if (topic == TOPIC_BC_GAS)
  {
    if (isOnPayload(payload))
      startGas(false);
    else
      stopGas(false);
    return;
  }

  if (topic == TOPIC_BC_PARTY)
  {
    if (isOnPayload(payload))
      startParty(false);
    else
      stopParty(false);
    return;
  }
}

void startGas(bool publish)
{
  currentGasStatus = ON;
  publishGas();

  // GAS hat Prio über Party
  printLcd("GASWARNUNG", "", false);


  rgbBlink(255, 0, 0, 500, 255);


  // wenn Storm aktiv ist beide Status anzeigen + Aktoren/Loops
  if (isStormActive())
  {
    printLcd("STURMWARNUNG &", "GASWARNUNG", false);
    Serial.println("[COMMON] ACHTUNG - GAS & STURM WARNUNG!!");
    refreshStormUi();
    return;
  }
    warnton();
}

void stopGas(bool publish)
{
  currentGasStatus = OFF;
  publishGas();

  rgbOff();
  buzzer.playTone(0, 0);

  // wenn Storm aktiv ist bleibt Status Sturmwarnung
  if (isStormActive())
  {
    printLcd("STURMWARNUNG", "", false);
    Serial.println("[COMMON] Gas ignored -keep STORM");
    refreshStormUi();
    return;
  }
  // wenn Party noch aktiv -> Party wieder anzeigen + Fenster zu
  if (currentPartyStatus == ON)
  {
    printLcd("PARTY BEI WOLFI!", "BIER UND METAL!", false);
    ctrWindow(WINDOW_CLOSED);
    return;
  }
  // LCD updaten
  refreshStormUi();
}

void startParty(bool publish)
{
  // Serial.println("[COMMON] startParty()");            // DEBUG !!

  if (isStormActive())
  {
    // STORM blockt party (Fenster wegen Sturm bereits zu)
    Serial.println("[COMMON] Party ignored - STURM");
    printLcd("STURMWARNUNG!!", "No Party! :(", false);
    return;
  }

  currentPartyStatus = ON;
  publishParty();
  // Serial.println("[COMMON] Party -> WINDOW_CLOSED");         // DEBUG !!
  ctrWindow(WINDOW_CLOSED);

  if (getGasStatus() == OFF)
  {
    printLcd("PARTY BEI WOLFI!", "BIER UND METAL!", false);
  }
}

void stopParty(bool publish)
{
  // Serial.println("[COMMON] stopParty()");            // DEBUG !!

  currentPartyStatus = OFF;
  publishParty();
  if (isStormActive()) // Backup & update display
  {
    // Serial.println("[COMMON] stopParty: keep STORM");         // DEBUG !!
    ctrWindow(WINDOW_CLOSED); // Backup - Fenster bleibt beim Storm geschlossen
    refreshStormUi();
    return;
  }
  if (currentGasStatus == ON)
  {
    // Gas hat Prio und bleibt am Display
    Serial.println("[COMMON] stopParty: keep Gaswarnung");
    return;
  }

  // Serial.println("[COMMON] Party OFF -> WINDOW_OPEN");        // DEBUG !!
  ctrWindow(WINDOW_OPEN);
  refreshStormUi();
}

Onoff getGasStatus() { return currentGasStatus; }
Onoff getPartyStatus() { return currentPartyStatus; }

void initCommon()
{
  publishGas();
  publishParty();
}