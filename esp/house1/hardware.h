#ifndef HARDWARE_H
#define HARDWARE_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <BuzzerESP32.h>

// -------------------- Pinbelegung (KS5009) --------------------
extern const uint8_t NEOPIXEL_COUNT;    
extern const uint8_t PIN_SERVO_WINDOW;       // Fenster-Servo
extern const uint8_t PIN_LED_YELLOW;         // gelbe LED am Haus
extern const uint8_t PIN_SERVO_DOOR;         // Tür-Servo
extern const uint8_t PIN_MOTION;             // PIR Bewegungsmelder
extern const uint8_t PIN_TEMPHUM;            // XHT11 Temperatur+Feuchte
extern const uint8_t PIN_FAN_PWM;            // PWM für Ventilator (Geschwindigkeit)
extern const uint8_t PIN_FAN_DIR;            // Direktionspin für Ventilator
extern const uint8_t PIN_BUZZER;             // Buzzer
extern const uint8_t PIN_LED_STRIP;          // SK6812 / NeoPixel
extern const uint8_t PIN_STEAM;              // Steam-Sensor

// Anzahl LEDs im NeoPixel-Strip (RGB-Modul)     
extern const uint8_t NEOPIXEL_COUNT;
// extern const uint8_t FAN_PWMCH;

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

// -------------------- Globale Hardware-Objekte --------------------
extern Adafruit_NeoPixel strip;  // SK6812 / NeoPixel
extern Servo windowServo;
extern Servo doorServo;
extern BuzzerESP32 buzzer;

// Fenster und Tür in definierte Zustände fahren
void initHardware();   
void ctrWindow(WindowState state);
void ctrDoor(DoorState state);

#endif
