#include <Arduino.h>
#include "hardware.h"
#include "driver/ledc.h"

// -------------------- Pinbelegung (KS5009-Standard) --------------------
const uint8_t FAN_PWMCH        =  0;
const uint8_t NEOPIXEL_COUNT   =  4;
const uint8_t PIN_SERVO_WINDOW =  5;   // Fenster-Servo
const uint8_t PIN_LED_YELLOW   = 12;  // gelbe LED am Haus
const uint8_t PIN_SERVO_DOOR   = 13;  // Tür-Servo
const uint8_t PIN_MOTION       = 14;
const uint8_t PIN_BTN2         = 16;
const uint8_t PIN_FAN_PWM      = 18;  // PulsWeitenModulation zur Geschwindigkeitssteuerung
const uint8_t PIN_FAN_DIR      = 19;  // Richtung: HIGH/LOW links/rechtsLauf
const uint8_t PIN_BUZZER       = 25;  // Buzzer
const uint8_t PIN_LED_STRIP    = 26;  // SK6812 / NeoPixel
const uint8_t PIN_BTN1         = 27;

static const uint8_t LED_COUNT = 4;  // Anzahl der Pixel im Strip (RGB)

// -------------------- Globale Objekte --------------------
Adafruit_NeoPixel strip(LED_COUNT, PIN_LED_STRIP, NEO_GRB + NEO_KHZ800);
Servo windowServo;
Servo doorServo;
BuzzerESP32 buzzer(PIN_BUZZER);

// weil die Servos zu Überhitzung neigen, insbesondere das Fenster
const bool USE_DOOR     = true;
const bool USE_WINDOW   = false;

// -------------------- Initialisierung --------------------
void initHardware() {
    // LED
    pinMode(PIN_LED_YELLOW, OUTPUT);
    digitalWrite(PIN_LED_YELLOW, LOW);

    // Servos vorbereiten (Standard 50 Hz)
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    windowServo.setPeriodHertz(50);
    doorServo.setPeriodHertz(50);

    // Min/Max-Pulse ggf. feinjustieren
    windowServo.attach(PIN_SERVO_WINDOW, 1000, 2000);
    doorServo.attach(PIN_SERVO_DOOR,   1000, 2000);

    // NeoPixel
    strip.begin();
    strip.clear();
    strip.show();

    // Buzzer
    buzzer.setTimbre(30);      // Klangfarbe (Keyestudio-Beispiel)
    buzzer.playTone(0, 0);     // sicherstellen, dass er aus ist

    // Fan
    pinMode(PIN_FAN_DIR, OUTPUT);              // FAN_DIR_PIN ist ein digitaler Ausgang für Richtung od. ein/aus

    // Buttons
    pinMode(PIN_BTN1, INPUT_PULLUP);
    pinMode(PIN_BTN2, INPUT_PULLUP);
}

// -------------------- Helper für Fenster / Tür --------------------
// Hinweis: Aus dem Keyestudio-Beispiel: 0° ~ „zu“, 176° ~ „auf“ für Fenster. :contentReference[oaicite:1]{index=1}
void ctrWindow(WindowState state) {
    if (!USE_WINDOW) return;
    int angle = 0;
    switch (state) {
        case WINDOW_OPEN:
            angle = 176;   // ganz auf
            break;
        case WINDOW_CLOSED:
        default:
            angle = 0;     // ganz zu
            break;
    }
    windowServo.write(angle);
}

void ctrFan(FanState state) {
  switch (state) {
    case FAN_ON:
      digitalWrite(PIN_FAN_DIR, HIGH);
      analogWrite(PIN_FAN_PWM, 255);
      delay(200);               // kurz anlaufen lassen
      analogWrite(PIN_FAN_PWM, 180);
      break;

    case FAN_OFF:
      digitalWrite(PIN_FAN_DIR, LOW);  
      analogWrite(PIN_FAN_PWM, 0);
      break;
    default:
      break;
  }
}


// Für die Tür kannst du die Winkel nach Bedarf justieren.
// Hier: 0° = zu, 90° = auf (oder 180°, wenn es mechanisch besser passt).
void ctrDoor(DoorState state) {
    if (!USE_DOOR) return;
    int angle = 0;
    switch (state) {
        case DOOR_OPEN:
            angle = 90;    // oder 180, je nach Mechanik
            break;
        case DOOR_CLOSED:
        default:
            angle = 0;
            break;
    }
    doorServo.write(angle);
}
