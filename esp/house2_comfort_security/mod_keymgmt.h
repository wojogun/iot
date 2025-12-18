#ifndef KEYMGMT_H
#define KEYMGMT_H

#include <Arduino.h>

void initRFID();
void loopRFID();

void initRuntimeConfig();
String getRfidKey();
bool setRfidKey(const String& key);


#endif