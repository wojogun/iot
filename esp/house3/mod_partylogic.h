#ifndef PARTYLOGIC_H
#define PARTYLOGIC_H
#include <Arduino.h>

enum Mode {
  MODE_NORMAL,
  MODE_PARTY
//  MODE_STORM,
//  MODE_GAS
};
enum Onoff {
  ON,
  OFF
};

extern String nextPartyText;
extern Mode currentMode;
extern Onoff currentGasStatus;
extern Onoff currentStormStatus;

// Haus-spezifische MQTT-Parameter für Haus 3
extern const char* MQTT_CLIENT_ID_BASE;
extern const char* TOPIC_BC_PARTY;
extern const char* TOPIC_BC_STORM;
extern const char* TOPIC_CMD_PARTY;
extern const char* TOPIC_CMD_STORM;
extern const char* TOPIC_CMD_SONG;
extern const char* TOPIC_CMD_NEXT;
extern const char* TOPIC_STATUS_HOUSE3;
extern const char* TOPIC_CURRENT_SONG;
void handleMqtt(const String& topic, const String& payload);
void onMotion(bool active);

void initParty();
void loopParty();

void controlParty(Mode mode, bool publish);
void startStorm(bool publish);
void stopStorm(bool publish);
void startGas(bool publish);
void stopGas(bool publish);
void printWarnings();

#endif