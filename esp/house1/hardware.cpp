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

// weil die Servos zu Überhitzung neigen, insbesondere das Fenster
const bool USE_DOOR     = true;
const bool USE_WINDOW   = false;



// -------------------- Initialisierung --------------------
void initHardware() {
    // LED
    pinMode(PIN_LED_YELLOW, OUTPUT);
    digitalWrite(PIN_LED_YELLOW, LOW);
    //initRgb();

    // Servos vorbereiten (Standard 50 Hz)
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);

    windowServo.setPeriodHertz(50);
    doorServo.setPeriodHertz(50);

    // Min/Max-Pulse ggf. feinjustieren
    // windowServo.attach(PIN_SERVO_WINDOW, 1000, 2000);
    // doorServo.attach(PIN_SERVO_DOOR,   1000, 2000);

    // FAN
    initFan();
    // pinMode(PIN_FAN_PWM, OUTPUT);     // Informiert ESP32 - dieser Pin steuert ein Signal aus
    // pinMode(PIN_FAN_DIR, OUTPUT);
    // analogWrite(PIN_FAN_PWM, 0);      // Fan bekommt kein "Speed"-Impuls
    // digitalWrite(PIN_FAN_DIR, LOW);   // Setzt Richtung/Enable Leitung auf eine definierte Zustand

    // BUTTONS
    pinMode(PIN_BTN1, INPUT_PULLUP);
    pinMode(PIN_BTN2, INPUT_PULLUP);

    // BUZZER
    buzzer.setTimbre(30);      // Klangfarbe (Keyestudio-Beispiel)
    buzzer.playTone(0, 0);     // sicherstellen, dass er aus ist

    // NeoPixel
    // strip.begin();
    // strip.clear();
    // strip.show();

    // SET DEFAULT SAFE STATE
    ctrFan(FAN_OFF);
    ctrDoor(DOOR_OPEN);
    ctrWindow(WINDOW_OPEN);

    switchLed(false);
    buzzer.playTone(0,0);
   // rgbOff();                
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
  //lichtorgelEnabled = false;
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
  //partyEnabled = false;
  rgbBlinkEnabled   = false;
  rgbStateOn        = false;
  rgbR = rgbG = rgbB = 0;
  rgbApplyColor(rgbR, rgbG, rgbB);
}

void rgbSet(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness) {
  //partyEnabled = false;
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

    //strip.setPixelColor(idx, c);
    strip.show();
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



// ====== HELPER WINDOW / DOOR / FANHelper für Fenster / Tür ========
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

// Für die Tür kannst du die Winkel nach Bedarf justieren.
// Hier: 0° = zu, 90° = auf (oder 180°, wenn es mechanisch besser passt).
// void ctrDoor(DoorState state) {
//     if (!USE_DOOR) return;
//     int angle = 0;
//     switch (state) {
//         case DOOR_OPEN:
//             angle = 90;    // oder 180, je nach Mechanik
//             break;
//         case DOOR_CLOSED:
//         default:
//             angle = 0;
//             break;
//     }
//     doorServo.write(angle);
// }
void ctrDoor(DoorState state) {
  if (!USE_DOOR) return;

  if (!doorServo.attached()) {
    doorServo.attach(PIN_SERVO_DOOR, 1000, 2000);
  }

  int angle = (state == DOOR_OPEN) ? 90 : 0;
  doorServo.write(angle);

  delay(300);        // kurz Zeit geben zum fahren
  doorServo.detach(); // dann Ruhe + weniger Hitze
}

// void ctrFan(FanState state) {
//   switch (state) {
//     case FAN_ON:
//       digitalWrite(PIN_FAN_DIR, HIGH);
//       analogWrite(PIN_FAN_PWM, 255);
//       delay(200);               
//       analogWrite(PIN_FAN_PWM, 180);
//       break;

//     case FAN_OFF:
//       digitalWrite(PIN_FAN_DIR, LOW);  
//       analogWrite(PIN_FAN_PWM, 0);
//       break;
//     default:
//       break;
//   }

void ctrFan(FanState state) {
  if (state == FAN_ON) {
    setFanPercent(70);   // default "ON" = 70%
  } else {
    setFanPercent(0);    // "OFF"
  }
}