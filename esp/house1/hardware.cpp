#include <Arduino.h>
#include "hardware.h"
#include "mod_sensor_fan.h"
#include "driver/ledc.h"

// -------------------- Pinbelegung (KS5009-Standard) --------------------
const uint8_t FAN_PWMCH         = 0;
const uint8_t NEOPIXEL_COUNT    = 4;
const uint8_t PIN_SERVO_WINDOW  = 5;     // Fenster-Servo
const uint8_t PIN_LED_YELLOW    = 12;    // gelbe LED am Haus
const uint8_t PIN_SERVO_DOOR    = 13;    // Tür-Servo
const uint8_t PIN_BTN1          = 16;    // Linker button (Gelb)
const uint8_t PIN_TEMP_HUM      = 17;    // XHT11 Temperatur+Feuchte
const uint8_t PIN_FAN_PWM       = 18;    // PWM für Ventilator (Geschwindigkeit) (IN-)
const uint8_t PIN_FAN_DIR       = 19;    // Direktionspin für Ventilator (IN+)
// const uint8_t PIN_GAS        = 23;    // Gas-Sensor 
const uint8_t PIN_BUZZER        = 25;    // Buzzer (Unter Fenster)
const uint8_t PIN_LED_STRIP     = 26;    // SK6812 / NeoPixel
const uint8_t PIN_BTN2          = 27;    // Rechter button beim Tür (Gelb)
const uint8_t PIN_STEAM         = 34;    // Steam-/Regen-/Nebel-Sensor (analog)



// -------------------- Globale Objekte --------------------
BuzzerESP32 buzzer(PIN_BUZZER);
Servo windowServo;
Servo doorServo;

// Fenster Winkel (kalibriert)
static const int WINDOW_OPEN_ANGLE  = 170;
static const int WINDOW_CLOSE_ANGLE = 60;

// Tür Winkel (kalibriert)
static const int DOOR_OPEN_ANGLE  = 100;
static const int DOOR_CLOSE_ANGLE = 50;

// Servo-Zeit zum "ankommen" (ms)
static const int SERVO_MOVE_DELAY_MS = 1000;
static void moveServoOnce(Servo& servo, uint8_t pin, int angle);

// RBG Leds 
static const uint8_t LED_COUNT  = 4;     // Anzahl der Pixel im Strip (RGB)
Adafruit_NeoPixel strip(LED_COUNT, PIN_LED_STRIP, NEO_GRB + NEO_KHZ800);
static bool rgbBlinkEnabled     = false;
static bool rgbStateOn          = false;
static uint32_t rgbLastToggle   = 0;
static uint32_t rgbInterval     = 500;
static uint8_t rgbR=0, rgbG=0, rgbB=0;
static uint8_t rgbBrightness    = 255;

// YELLOW LED
static bool     ledBlinkEnabled  = false;
static bool     ledState         = false;
static uint32_t ledBlinkInterval = 500;
static uint32_t ledLastToggle    = 0;

// SERVO (Ausschalten wenn überlasten bzw. nicht richtig funktioniert (Überhitzung))
const bool USE_DOOR     = true;
const bool USE_WINDOW   = true;


// -------------------- Initialisierung --------------------
void initHardware() {
    // LED
    pinMode(PIN_LED_YELLOW, OUTPUT);
    digitalWrite(PIN_LED_YELLOW, LOW);

    // RGB / NeoPixel AKTIVIEREN
    initRgb();

    // Servos vorbereiten (Standard 50 Hz)
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    windowServo.setPeriodHertz(50);
    doorServo.setPeriodHertz(50);

    // FAN
    initFan();

    // BUTTONS
    pinMode(PIN_BTN1, INPUT_PULLUP);
    pinMode(PIN_BTN2, INPUT_PULLUP);

    // BUZZER
    buzzer.setTimbre(30);     
    buzzer.playTone(0, 0);  

    // SET DEFAULT SAFE STATE
    ctrFan(FAN_OFF);
    ctrDoor(DOOR_OPEN);
    delay(300);
    ctrWindow(WINDOW_OPEN);

    switchLed(false);
    buzzer.playTone(0,0);
    rgbOff();                
}



// ==== BUZZER ====
void warnton() {
  for (uint16_t i = 0; i < 4; i++) {
    buzzer.playTone(500,1000);
    delay(500);
  }
  buzzer.playTone(0, 0);
}


// ==== YELLOW LED ====
void switchLed(bool onoff) {
  ledState          = onoff;
  ledBlinkEnabled   = false;
  digitalWrite(PIN_LED_YELLOW, (onoff ? HIGH : LOW));
}
void blinkLed() {
    ledBlinkEnabled  = true;
    ledLastToggle    = millis();
}
void loopYellowLed() {
  if (!ledBlinkEnabled) return;
  uint32_t now = millis();
  if (now - ledLastToggle >= ledBlinkInterval) {
    ledLastToggle = now;
    ledState = !ledState;
    digitalWrite(PIN_LED_YELLOW, ledState ? HIGH : LOW);
  }
}


// ==== RGB LED ====
void initRgb() {
  strip.begin();
  strip.clear();
  strip.setBrightness(255);
  strip.show();
  rgbBlinkEnabled = false;
  rgbStateOn = false;
}

void rgbApplyColor(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

void loopRgb() {
  uint32_t now = millis();
  // Blinken
  if (!rgbBlinkEnabled) return;

  if (now - rgbLastToggle >= rgbInterval) {
    rgbLastToggle = now;
    rgbStateOn = !rgbStateOn;
    if (rgbStateOn) rgbApplyColor(rgbR, rgbG, rgbB);
    else            rgbApplyColor(0,0,0);
  }
}

void rgbOff() {
  rgbBlinkEnabled   = false;
  rgbStateOn        = false;
  rgbR = rgbG = rgbB = 0;
  rgbApplyColor(rgbR, rgbG, rgbB);
}

void rgbSet(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
  rgbBlinkEnabled   = false;
  rgbR = r; rgbG = g; rgbB = b;
  rgbBrightness = brightness;

  strip.setBrightness(rgbBrightness);
  rgbStateOn = true;
  rgbApplyColor(rgbR, rgbG, rgbB);
}

void rgbSetHSV(uint16_t hue, uint8_t sat, uint8_t val) {
  strip.setBrightness(val);
  uint32_t c = strip.gamma32(strip.ColorHSV(hue, sat, val));

  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
    strip.show();
}

void rgbBlink(uint8_t r, uint8_t g, uint8_t b, uint32_t intervalMs, uint8_t brightness) {
  rgbBlinkEnabled = true;

  rgbR = r; rgbG = g; rgbB = b;
  rgbBrightness = brightness;
  rgbInterval = intervalMs;
  rgbLastToggle = millis();
  strip.setBrightness(rgbBrightness);
}

// ====== HELPER WINDOW / DOOR / FAN ========
static void moveServoOnce(Servo& servo, uint8_t pin, int angle) {
  if (!servo.attached()) {
    servo.attach(pin, 500, 2500);
  }
  servo.write(angle);
  delay(SERVO_MOVE_DELAY_MS);
  servo.detach();
}

void ctrDoor(DoorState state) {
  if (!USE_DOOR) return;

  static DoorState last = (DoorState)255;
  if (state == last) return;
  last = state;

  int angle = (state == DOOR_OPEN) ? DOOR_OPEN_ANGLE : DOOR_CLOSE_ANGLE;
  moveServoOnce(doorServo, PIN_SERVO_DOOR, angle);

  Serial.printf("[HW] ctrDoor -> %s\n", state == DOOR_OPEN ? "OPEN" : "CLOSED");
}


void ctrWindow(WindowState state) {
  if (!USE_WINDOW) return;

  static WindowState last = (WindowState)255;     // ungültig beim Start

  if (state == last) {
    return;               // nix zu tun ->  Dauer-Detach/Attach verhindert
  }
  last = state;

  int angle = (state == WINDOW_OPEN) ? WINDOW_OPEN_ANGLE : WINDOW_CLOSE_ANGLE;
  moveServoOnce(windowServo, PIN_SERVO_WINDOW, angle);

  Serial.printf("[HW] ctrWindow -> %s\n", state == WINDOW_OPEN ? "OPEN" : "CLOSED");
}

void ctrFan(FanState state) {
  if (state == FAN_ON) {
    setFanPercent(70);   // default "ON" = 70%
  } else {
    setFanPercent(0);    // "OFF"
  }
}

