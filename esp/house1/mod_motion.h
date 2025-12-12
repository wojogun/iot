#ifndef MOD_MOTION_H
#define MOD_MOTION_H

#include <Arduino.h>

void initMotion();
void loopMotion();

bool motionIsActive();
bool motionRising();
bool motionFalling();

void registerMotionCallback(void (*onChange)(bool));

#endif
