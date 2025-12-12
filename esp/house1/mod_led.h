#ifndef MOD_LED_H
#define MOD_LED_H

#include <Arduino.h>

void initHouseLed();
void setHouseLed(bool on);
void blinkHouseLed(uint8_t times, uint16_t onMs = 200, uint16_t offMs = 200);

#endif
