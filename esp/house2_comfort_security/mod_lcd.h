#pragma once
#include <Arduino.h>

void initLcd();
void printLcd(const String& oben, const String& unten);
void printLcd(const String& oben, const String& unten, bool flash = false);
void printTempLcd(const String& oben, const String& unten, unsigned long dur);
void lcdUpdate();
