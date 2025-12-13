#include <Arduino.h>
#include "hardware.h"
#include "driver/ledc.h"

// -------------------- Pinbelegung (KS5009-Standard) --------------------
const uint8_t FAN_PWMCH        =  0;
const uint8_t NEOPIXEL_COUNT   =  4;
const uint8_t PIN_SERVO_WINDOW =  5;  // Fenster-Servo
const uint8_t PIN_LED_YELLOW   = 12;  // gelbe LED am Haus
const uint8_t PIN_SERVO_DOOR   = 13;  // Tür-Servo
const uint8_t PIN_MOTION       = 14;
const uint8_t PIN_BTN2         = 16;
const uint8_t PIN_FAN_PWM      = 18;  // PulsWeitenModulation zur Geschwindigkeitssteuerung
const uint8_t PIN_FAN_DIR      = 19;  // Richtung: HIGH/LOW links/rechtsLauf
const uint8_t PIN_BUZZER       = 25;  // Buzzer
const uint8_t PIN_LED_STRIP    = 26;  // SK6812 / NeoPixel
const uint8_t PIN_BTN1         = 27;


// -------------------- Globale Objekte --------------------
BuzzerESP32 buzzer(PIN_BUZZER);
Servo windowServo;
Servo doorServo;

// RGBLED
static const uint8_t LED_COUNT = 4;  // Anzahl der Pixel im Strip (RGB)
Adafruit_NeoPixel strip(LED_COUNT, PIN_LED_STRIP, NEO_GRB + NEO_KHZ800);
static bool rgbBlinkEnabled = false;
static bool rgbStateOn      = false;
static uint32_t rgbLastToggle = 0;
static uint32_t rgbInterval   = 500;
static uint8_t rgbR=0, rgbG=0, rgbB=0;
static uint8_t rgbBrightness = 255;

// gelbe LED
static bool     ledBlinkEnabled  = false;
static bool     ledState         = false;
static uint32_t ledBlinkInterval = 500;
static uint32_t ledLastToggle    = 0;

// weil die Servos zu Überhitzung neigen, insbesondere das Fenster
const bool USE_DOOR     = true;
const bool USE_WINDOW   = false;

// -------------------- Initialisierung --------------------
void initHardware() {
    // LED
    pinMode(PIN_LED_YELLOW, OUTPUT);
    digitalWrite(PIN_LED_YELLOW, LOW);
    initRgb();

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

    // Buzzer
    buzzer.setTimbre(30);      // Klangfarbe (Keyestudio-Beispiel)
    buzzer.playTone(0, 0);     // sicherstellen, dass er aus ist

    // Fan
    pinMode(PIN_FAN_DIR, OUTPUT); // FAN_DIR_PIN ist ein digitaler Ausgang für Richtung od. ein/aus

    // Buttons
    pinMode(PIN_BTN1, INPUT_PULLUP);
    pinMode(PIN_BTN2, INPUT_PULLUP);
}

void initRgb() {
  strip.begin();
  strip.clear();
  strip.setBrightness(255);
  strip.show();
  rgbBlinkEnabled = false;
  //lichtorgelEnabled = false;
  rgbStateOn = false;
}
void loopRgb() {
  uint32_t now = millis();

  // Party hat Priorität (falls aktiv)
  /*
  if (lichtorgelEnabled) {
    if (now - partyLast >= partyStepMs) {
      partyLast = now;
      partyHue += 256; // Geschwindigkeit/Farbwechsel
      // Wenn ColorHSV/gamma32 nicht verfügbar: sag Bescheid, dann gebe ich Fallback ohne HSV
      uint32_t c = strip.gamma32(strip.ColorHSV(partyHue));
      for (uint16_t i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, c);
      strip.show();
    }
    return;
  }
  */

  // Blinken
  if (!rgbBlinkEnabled) return;

  if (now - rgbLastToggle >= rgbInterval) {
    rgbLastToggle = now;
    rgbStateOn = !rgbStateOn;
    if (rgbStateOn) rgbApplyColor(rgbR, rgbG, rgbB);
    else            rgbApplyColor(0,0,0);
  }
}

void rgbApplyColor(uint8_t r, uint8_t g, uint8_t b) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();
}

void rgbOff() {
  //partyEnabled = false;
  rgbBlinkEnabled = false;
  rgbStateOn = false;
  rgbR = rgbG = rgbB = 0;
  rgbApplyColor(rgbR, rgbG, rgbB);
}

void rgbSet(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
  //partyEnabled = false;
  rgbBlinkEnabled = false;
  rgbR = r; rgbG = g; rgbB = b;
  rgbBrightness = brightness;

  strip.setBrightness(rgbBrightness);
  rgbStateOn = true;
  rgbApplyColor(rgbR, rgbG, rgbB);
}

void rgbBlink(uint8_t r, uint8_t g, uint8_t b, uint32_t intervalMs, uint8_t brightness) {
  //partyEnabled = false;
  rgbBlinkEnabled = true;

  rgbR = r; rgbG = g; rgbB = b;
  rgbBrightness = brightness;
  rgbInterval = intervalMs;
  rgbLastToggle = millis();
  strip.setBrightness(rgbBrightness);
}






void switchLed(bool onoff) {
  ledState = onoff;
  ledBlinkEnabled = false;
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


