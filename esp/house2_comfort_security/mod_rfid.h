#ifndef RFID_H
#define RFID_H

#include <Arduino.h>

void initRFID();
void loopRFID();

void initRuntimeConfig();
String getRfidKey();
bool setRfidKey(const String& key);


#endif