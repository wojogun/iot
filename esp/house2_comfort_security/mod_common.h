#ifndef COMMON_H
#define COMMON_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <BuzzerESP32.h>

enum Onoff {
  ON,
  OFF
};

// -------------------- Pinbelegung (KS5009) --------------------
extern const uint8_t PIN_LED_YELLOW;
extern const uint8_t PIN_LED_STRIP;
extern const uint8_t PIN_BUZZER;
extern const uint8_t PIN_FAN_PWM;
extern const uint8_t PIN_FAN_DIR;
extern const uint8_t PIN_BTN1;
extern const uint8_t PIN_BTN2;
extern const uint8_t PIN_MOTION;

// Anzahl LEDs im NeoPixel-Strip (RGB-Modul)
extern const uint8_t NEOPIXEL_COUNT;
extern const uint8_t FAN_PWMCH;

extern const char* TOPIC_STATUS_HOUSE2;
extern const char* TOPIC_STATUSGAS_HOUSE2;
extern const char* TOPIC_STATUSSTORM_HOUSE2;
extern const char* TOPIC_BC_STORM;
extern const char* TOPIC_BC_PARTY;
extern const char* TOPIC_BC_GAS;

// -------------------- Globale Hardware-Objekte --------------------
extern Adafruit_NeoPixel strip;  // SK6812 / NeoPixel
extern BuzzerESP32 buzzer;

void initRgb();
void loopRgb();
void rgbApplyColor(uint8_t r, uint8_t g, uint8_t b);
void rgbOff();
void rgbSet(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness);
void rgbSetHSV(uint16_t hue, uint8_t sat, uint8_t val);
void rgbBlink(uint8_t r, uint8_t g, uint8_t b, uint32_t intervalMs=500, uint8_t brightness=240);

void switchLed(bool onoff);
void blinkLed();
void loopYellowLed();

void handleMqtt(const String& topic, const String& payload);

void InitCommon();

void startGas(bool publish);
void stopGas(bool publish);
void startStorm(bool publish);
void stopStorm(bool publish);
void printWarnings();

#endif