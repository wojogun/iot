#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <BuzzerESP32.h>

// -------------------- Pinbelegung (KS5009) --------------------
extern const uint8_t PIN_SERVO_WINDOW;       // Fenster-Servo
extern const uint8_t PIN_LED_YELLOW;         // gelbe LED am Haus
extern const uint8_t PIN_SERVO_DOOR;         // Tür-Servo
extern const uint8_t PIN_BTN1;               // Linker button
extern const uint8_t PIN_TEMP_HUM;            // XHT11 Temperatur+Feuchte
extern const uint8_t PIN_FAN_PWM;            // PWM für Ventilator (Geschwindigkeit)
extern const uint8_t PIN_FAN_DIR;            // Direktionspin für Ventilator
extern const uint8_t PIN_BUZZER;             // Buzzer
extern const uint8_t PIN_LED_STRIP;          // SK6812 / NeoPixel
extern const uint8_t PIN_BTN2;               // Rechter button
extern const uint8_t PIN_STEAM;              // Steam-Sensor

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


// Globaler Text für "Aktuelles Wetter" (weather_logic)
extern String actualWeather;


// Fenster und Tür in definierte Zustände fahren
void initHardware();   
void ctrWindow(WindowState state);
void ctrDoor(DoorState state);
void ctrFan(FanState state);

// ==== LED / BUZZER helper (used by logic modules) ====
void warnton();

// Yellow LED
void switchLed(bool onoff);
void blinkLed();
void loopYellowLed();

// RGB (optional - only if you use it somewhere)
void initRgb();
void loopRgb();
void rgbOff();
void rgbSet(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness = 255);
void rgbBlink(uint8_t r, uint8_t g, uint8_t b, uint32_t intervalMs, uint8_t brightness = 255);
void rgbSetHSV(uint16_t hue, uint8_t sat, uint8_t val);

#endif
