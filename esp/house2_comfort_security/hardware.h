#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <BuzzerESP32.h>

// -------------------- Pinbelegung (KS5009) --------------------
extern const uint8_t PIN_LED_YELLOW;
extern const uint8_t PIN_SERVO_WINDOW;
extern const uint8_t PIN_SERVO_DOOR;
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

extern const bool USE_DOOR;
extern const bool USE_WINDOW;

// -------------------- Zustände für Fenster / Tür --------------------
enum WindowState {
    WINDOW_CLOSED = 0,
    WINDOW_OPEN   = 1
};

enum DoorState {
    DOOR_CLOSED = 0,
    DOOR_OPEN   = 1
};

enum FanState {
    FAN_OFF = 0,
    FAN_ON  = 1
};

// -------------------- Globale Hardware-Objekte --------------------
extern Adafruit_NeoPixel strip;  // SK6812 / NeoPixel
extern Servo windowServo;
extern Servo doorServo;
extern BuzzerESP32 buzzer;

// Globaler Text für "Nächste Party" (von MQTT & Partylogic)
extern String nextPartyText;

// Fenster und Tür in definierte Zustände fahren
void initHardware();
void ctrWindow(WindowState state);
void ctrDoor(DoorState state);
void ctrFan(FanState state);

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

#endif
