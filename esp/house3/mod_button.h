#ifndef MOD_BUTTON_H
#define MOD_BUTTON_H

#include <Arduino.h>

enum ButtonEvent {
  BUTTON_NONE,
  BUTTON_SHORT,
  BUTTON_LONG,
  BUTTON_DOUBLE
};

struct ButtonState {
  uint8_t  pin;
  bool     activeLow;       // true = gedrückt bei LOW (INPUT_PULLUP)
  uint32_t longPressMs;     // Schwelle für LONG
  uint32_t doubleClickMs;   // Zeitfenster für Doppelklick
  uint32_t debounceMs;      // Entprellzeit

  // interne Zustände
  bool     pressed;
  uint32_t lastChangeTime;
  uint32_t pressStartTime;
  uint32_t lastReleaseTime;
  bool     pendingShort;
  bool     longReported;
};

extern ButtonState btn1;
extern ButtonState btn2;

void initButton(ButtonState& btn);
ButtonEvent updateButton(ButtonState& btn, uint32_t nowMs);

#endif
