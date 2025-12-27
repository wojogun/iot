#ifndef MOD_LOGIC_COMMON_H
#define MOD_LOGIC_COMMON_H

#include <Arduino.h>

enum Onoff { OFF = 0, ON = 1 };

void InitCommon();

// MQTT handler (wird vom Dispatcher aufgerufen)
void handleMqttCommon(const String& topic, const String& payload);

// Events
void startGas(bool publish);
void stopGas(bool publish);

void startParty(bool publish);
void stopParty(bool publish);

// Status getter (optional, aber praktisch)
Onoff getGasStatus();
Onoff getPartyStatus();

#endif
