#include "mod_motion.h"
#include "hardware.h"

// TODO: später bei motionRising() z.B. LED blinken lassen, Buzzer, MQTT-Alarm....

static bool motionActiveHigh  = true;   // HIGH = Bewegung
static bool usePullup         = false;  // INPUT vs INPUT_PULLUP
static bool motionState       = false;
static bool lastState         = false;
static unsigned long lastChangeMs = 0;
static const unsigned long debounceMs = 50;

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

    if (active != lastState) {
        lastState    = active;
        lastChangeMs = now;
    }

    if ((now - lastChangeMs) >= debounceMs) {
        if (motionState != lastState) {
            motionState = lastState;
            risingLatched  = motionState;
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
