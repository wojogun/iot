#ifndef RFID_H
#define RFID_H
#include <Arduino.h>

typedef void (*RfidCallback)(const String& uid);
void initRFID();
void loopRfid();

// Haus-/Anwendungslogik registriert hier ihren Handler
void registerRfidCallback(RfidCallback cb);

#endif