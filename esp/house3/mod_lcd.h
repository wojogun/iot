#ifndef MOD_LCD_H
#define MOD_LCD_H
#include <Arduino.h>

void initLcd();
void print(const String& oben, const String& unten);
void printLcd(const String& oben, const String& unten, bool flash = false);
void printTempLcd(const String& oben, const String& unten, unsigned long dur);
void loopLcd();

#endif