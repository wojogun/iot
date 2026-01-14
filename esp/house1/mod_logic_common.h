#ifndef MOD_LOGIC_COMMON_H
#define MOD_LOGIC_COMMON_H

#include <Arduino.h>

enum Onoff { OFF = 0, ON = 1 };

void initCommon();

// MQTT handler Broadcast Gas & Party
void handleMqttCommon(const String& topic, const String& payload);

// Events
void startGas(bool publish);
void stopGas(bool publish);

void startParty(bool publish);
void stopParty(bool publish);

// Status getter
Onoff getGasStatus();
Onoff getPartyStatus();

#endif
