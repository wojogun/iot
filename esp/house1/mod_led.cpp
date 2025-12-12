#include "mod_led.h"
#include "hardware.h"

void initHouseLed() {
    pinMode(PIN_LED_YELLOW, OUTPUT);
    digitalWrite(PIN_LED_YELLOW, LOW);
}

void setHouseLed(bool on) {
    digitalWrite(PIN_LED_YELLOW, on ? HIGH : LOW);
}

void blinkHouseLed(uint8_t times, uint16_t onMs, uint16_t offMs) {
    for (uint8_t i = 0; i < times; ++i) {
        setHouseLed(true);
        delay(onMs);
        setHouseLed(false);
        delay(offMs);
    }
}
