#ifndef MOD_CONFIG_WIFI_H
#define MOD_CONFIG_WIFI_H

#include <Arduino.h>
//void initSerial();

void initWiFi();
void wiFiLoop();
bool wiFiIsConnected();

#endif