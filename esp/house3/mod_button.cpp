#include "mod_button.h"

void initButton(ButtonState& btn) {
  pinMode(btn.pin, btn.activeLow ? INPUT_PULLUP : INPUT);
  btn.pressed         = false;
  btn.lastChangeTime  = 0;
  btn.pressStartTime  = 0;
  btn.lastReleaseTime = 0;
  btn.pendingShort    = false;
  btn.longReported    = false;
}

ButtonEvent updateButton(ButtonState& btn, uint32_t nowMs) {
  ButtonEvent ev = BUTTON_NONE;

  // 1) ggf. wartenden Short-Click auslösen, wenn Double-Click-Fenster abgelaufen
  if (btn.pendingShort && (nowMs - btn.lastReleaseTime > btn.doubleClickMs)) {
    btn.pendingShort = false;
    ev = BUTTON_SHORT;
  }

  int rawLevel   = digitalRead(btn.pin);
  bool isPressed = btn.activeLow ? (rawLevel == LOW) : (rawLevel == HIGH);

  // 2) Entprellung
  if (isPressed != btn.pressed) {
    if (nowMs - btn.lastChangeTime < btn.debounceMs) {
      return ev; // noch prellen, nichts tun
    }
    btn.lastChangeTime = nowMs;
    btn.pressed = isPressed;

    if (isPressed) {
      // gedrückt
      btn.pressStartTime = nowMs;
      btn.longReported   = false;
    } else {
      // losgelassen
      uint32_t dur = nowMs - btn.pressStartTime;

      if (!btn.longReported) {
        // Kandidat für kurz/doppelt
        if (dur < btn.longPressMs) {
          if (btn.pendingShort &&
              (nowMs - btn.lastReleaseTime <= btn.doubleClickMs)) {
            // zweiter Klick im Fenster → Doppelclick
            btn.pendingShort = false;
            ev = BUTTON_DOUBLE;
          } else {
            // erster kurzer Klick → merken, aber noch nicht sofort als SHORT melden
            btn.pendingShort    = true;
            btn.lastReleaseTime = nowMs;
          }
        }
        // wenn dur >= longPressMs und longReported==false, dann wurde
        // LONG vorher im Halten bereits gemeldet (siehe unten),
        // hier ist dann nichts mehr zu tun
      }
    }
  }

  // 3) Long-Press während Halten erkennen
  if (btn.pressed && !btn.longReported) {
    if (nowMs - btn.pressStartTime >= btn.longPressMs) {
      btn.longReported = true;
      btn.pendingShort = false; // kein Short mehr
      ev = BUTTON_LONG;
    }
  }

  return ev;
}
