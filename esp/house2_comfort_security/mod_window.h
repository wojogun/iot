#ifndef MOD_WINDOW_H
#define MOD_WINDOW_H

#include <Arduino.h>
//void initSerial();

void initWindow();
void openWindow();
void closeWindow();

extern const char* TOPIC_STATUS_WINDOW;

#endif