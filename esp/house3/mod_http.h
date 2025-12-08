#pragma once
#include <Arduino.h>

String buildRootPage(const String& ip, const String& mode, const String& msg);

void initHttp();
void loopHttp();