#include <Arduino.h>
#include "mod_logic_common.h"
#include "config.h"
#include "mod_config_mqtt.h"
#include "hardware.h"
#include "mod_sensor_lcd.h"   // falls du printLcd hier nutzen willst

static Onoff currentGasStatus   = OFF;
static Onoff currentPartyStatus = OFF;

static bool isOnPayload(const String& payload) {
  String p = payload; p.trim(); p.toUpperCase();
  return (p == "ON" || p == "1" || p == "TRUE" || p == "YES" || p == "PARTY");
}

static void publishGas() {
  publishMqtt(TOPIC_STATUSGAS_HOUSE1, currentGasStatus == ON ? "ON" : "OFF", true);
}

static void publishParty() {
 publishMqtt(TOPIC_STATUSPARTY_HOUSE1, currentPartyStatus == ON ? "ON" : "OFF", true);
}

static void printWarnings() {
  // Minimal: nur Gas/Party anzeigen (Storm macht dein Storm-Modul)
  uint8_t warnMask =
      (currentGasStatus   == ON ? 0b01 : 0) |
      (currentPartyStatus == ON ? 0b10 : 0) | (currentPartyStatus == ON ? 0b100 : 0);

  switch (warnMask) {
    case 0b00:
      // nichts – WeatherLogic zeigt ja normalen Screen
      break;
    case 0b01:
      printLcd("GASWARNUNG", "", false);
      break;
    case 0b10:
      printLcd("PARTY", "MODE", false);
      break;
    case 0b11:
      printLcd("GAS + PARTY", "WARNUNG", false);
      break;
    case 0b100:
      printLcd("WOLFI PSSSSST", "SEI BITTE LEISER!!! :D", false);
      break;
  }
}

void handleMqttCommon(const String& topic, const String& payload) {
  // Broadcast GAS
  if (topic == TOPIC_BC_GAS) {
    if (isOnPayload(payload)) startGas(false);
    else stopGas(false);
    return;
  }

  // Broadcast PARTY
  if (topic == TOPIC_BC_PARTY) {
    if (isOnPayload(payload)) startParty(false);
    else stopParty(false);
    return;
  }
}

void startGas(bool publish) {
  currentGasStatus = ON;
  publishGas();
  printWarnings();

  // optional: Effekte
  // warnton(); oder rgbBlink rot etc.
}

void stopGas(bool publish) {
  currentGasStatus = OFF;
  publishGas();
  printWarnings();
}

void startParty(bool publish) {
  currentPartyStatus = ON;
  publishParty();
  printWarnings();
}

void stopParty(bool publish) {
  currentPartyStatus = OFF;
  publishParty();
  printWarnings();
}

Onoff getGasStatus() { return currentGasStatus; }
Onoff getPartyStatus() { return currentPartyStatus; }

void InitCommon() {
  // subscribe broadcast topics
  subscribeMqtt(TOPIC_BC_GAS);
  subscribeMqtt(TOPIC_BC_PARTY);

  // initial publish damit UI richtig steht
  publishGas();
  publishParty();
}
