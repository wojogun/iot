#ifndef MOD_MOTION_H
#define MOD_MOTION_H

#include <Arduino.h>

void initMotion();
void loopMotion();

bool motionIsActive(); // aktuell stabil erkannten Zustand (true = Bewegung)
bool motionRising(); // Liefert genau EINMAL true, wenn gerade von "keine Bewegung" → "Bewegung" gewechselt wurde
bool motionFalling(); // Liefert genau EINMAL true, wenn gerade von "Bewegung" → "keine Bewegung" gewechselt wurde

// Callback registrieren, der bei Zustandsänderung aufgerufen wird
// onChange(newState): newState = true (Bewegung), false (keine Bewegung)
void registerMotionCallback(void (*onChange)(bool));

#endif
