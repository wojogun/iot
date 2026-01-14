// WIND = SIMULATION
// FAN = AKTOR !! 

#include <Arduino.h>
#include "hardware.h"
#include "mod_sensor_fan.h"

// ================== SETTINGS ==================
static const bool FAN_PWM_INVERTED = true;

// Kickstart
static const int  KICK_MS   = 180;
static const int  KICK_PWM  = 255;

// Minimal-PWM damit er sicher anläuft --> 0 = deaktiviert.
static const int  MIN_PWM_WHEN_ON = 0;
// ==============================================

static int fanPct = 0;

static inline int clampInt(int v, int lo, int hi) {
  if (v < lo) return lo;
  if (v > hi) return hi;
  return v;
}

static inline int pctToPwm(int pct) {
  pct = clampInt(pct, 0, 100);
  int pwm = map(pct, 0, 100, 0, 255);

  if (pct > 0 && MIN_PWM_WHEN_ON > 0 && pwm < MIN_PWM_WHEN_ON) pwm = MIN_PWM_WHEN_ON;

  if (FAN_PWM_INVERTED) pwm = 255 - pwm;
  return pwm;
}

void initFan() {
  pinMode(PIN_FAN_DIR, OUTPUT);
  pinMode(PIN_FAN_PWM, OUTPUT);

  digitalWrite(PIN_FAN_DIR, LOW);
  analogWrite(PIN_FAN_PWM, 0);

  fanPct = 0;
}

void setFanPercent(int pct) {
  pct = clampInt(pct, 0, 100);

  int prev = fanPct;
  fanPct = pct;

  if (pct == 0) {
    digitalWrite(PIN_FAN_DIR, LOW);
    analogWrite(PIN_FAN_PWM, 0);
    return;
  }

  // ON
  digitalWrite(PIN_FAN_DIR, HIGH);

  int pwm = pctToPwm(pct);

  // Kickstart nur wenn vorher OFF
  if (prev == 0) {
    analogWrite(PIN_FAN_PWM, KICK_PWM);
    delay(KICK_MS);
  }

  analogWrite(PIN_FAN_PWM, pwm);
}

int getFanPercent() {
  return fanPct;
}