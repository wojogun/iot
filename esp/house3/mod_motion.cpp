#include <Arduino.h>
#include "hardware.h"

// interne Zustände
static bool motionActiveHigh  = true;   // true  → Sensor liefert HIGH bei Bewegung, false → Sensor liefert LOW  bei Bewegung
static bool usePullup         = false;  // true → INPUT_PULLUP, false → normaler INPUT
static bool motionState       = false;  // aktueller stabiler Zustand
static bool lastState         = false;
static unsigned long lastChangeMs = 0;
static const unsigned long debounceMs = 50; // Entprellzeit in ms

static bool risingLatched  = false;
static bool fallingLatched = false;

static void (*motionCallback)(bool) = nullptr;

void initMotion() {
    pinMode(PIN_MOTION, usePullup ? INPUT_PULLUP : INPUT);
    int raw = digitalRead(PIN_MOTION);
    bool active = motionActiveHigh ? (raw == HIGH) : (raw == LOW);

    motionState  = active;
    lastState    = active;
    lastChangeMs = millis();
    risingLatched  = false;
    fallingLatched = false;
}

void loopMotion() {
    unsigned long now = millis();
    int raw = digitalRead(PIN_MOTION);
    bool active = motionActiveHigh ? (raw == HIGH) : (raw == LOW);
    //Serial.print("motion:");
    //Serial.println(String(raw));
    
    // Wenn sich der Rohzustand geändert hat, warte debounceMs, bevor wir es übernehmen
    if (active != lastState) {
        // Zustand hat sich verändert → Timer neu setzen
        lastState    = active;
        lastChangeMs = now;
    }

    // erst wenn der neue Zustand länger als debounceMs stabil ist, übernehmen
    if ((now - lastChangeMs) >= debounceMs) {
        if (motionState != lastState) {
            motionState = lastState;
            risingLatched = motionState;
            fallingLatched = !motionState;
            if (motionCallback) motionCallback(motionState);
        }
    }
}

bool motionIsActive() { return motionState; }

bool motionRising() {
    if (risingLatched) {
        risingLatched = false;
        return true;
    }
    return false;
}

bool motionFalling() {
    if (fallingLatched) {
        fallingLatched = false;
        return true;
    }
    return false;
}

void registerMotionCallback(void (*onChange)(bool)) {
    motionCallback = onChange;
}
