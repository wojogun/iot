#include <Arduino.h>
#include "hardware.h"
#include "driver/ledc.h"

// -------------------- Pinbelegung (KS5009-Standard) --------------------
const uint8_t PIN_SERVO_WINDOW =  5;  // Fenster-Servo
const uint8_t PIN_LED_YELLOW   = 12;  // gelbe LED am Haus
const uint8_t PIN_SERVO_DOOR   = 13;  // Tür-Servo
const uint8_t PIN_MOTION       = 14;
const uint8_t PIN_BTN2         = 16;
const uint8_t PIN_DHT11        = 17; // Temperature and humidity sensor pin
const uint8_t PIN_FAN_INB      = 18;  
const uint8_t PIN_FAN_INA      = 19;  
const uint8_t PIN_BUZZER       = 25;  // Buzzer
const uint8_t PIN_LED_STRIP    = 26;  // SK6812 / NeoPixel
const uint8_t PIN_BTN1         = 27;

const uint8_t NEOPIXEL_COUNT   =  4;
const uint8_t FAN_CH           =  0;

dht11 DHT11; // Initialize dht11
static unsigned long lastDhtRead = 0;
static Climate lastClimate;

static uint8_t fanSpeed = 0;

// -------------------- Globale Objekte --------------------
BuzzerESP32 buzzer(PIN_BUZZER);
Servo windowServo;
Servo doorServo;

// RGBLED
static const uint8_t LED_COUNT = 4;  // Anzahl der Pixel im Strip (RGB)
Adafruit_NeoPixel strip(LED_COUNT, PIN_LED_STRIP, NEO_GRB + NEO_KHZ800);
static bool     rgbBlinkEnabled = false;
static bool     rgbStateOn      = false;
static uint32_t rgbLastToggle = 0;
static uint32_t rgbInterval   = 500;
static uint8_t  rgbR=0, rgbG=0, rgbB=0;
static uint8_t  rgbBrightness = 255;

// gelbe LED
static bool     ledBlinkEnabled  = false;
static bool     ledState         = false;
static uint32_t ledBlinkInterval = 500;
static uint32_t ledLastToggle    = 0;

// weil die Servos zu Überhitzung neigen, insbesondere das Fenster
const bool USE_DOOR     = true;
const bool USE_WINDOW   = true; //offenbar ist servo bei haus 3 des resorts (ges. Haus 11) defekt

// -------------------- Initialisierung --------------------
void initHardware() {
    // LED
    pinMode(PIN_LED_YELLOW, OUTPUT);
    digitalWrite(PIN_LED_YELLOW, LOW);
    initRgb();
    initFan();

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
void initFan() {
  pinMode(PIN_FAN_INA, OUTPUT);
  pinMode(PIN_FAN_INB, OUTPUT);
  digitalWrite(PIN_FAN_INA, LOW);
  analogWrite(PIN_FAN_INB, 0); 
}

// ----------------- loops -------------------------------
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
void loopTempHum() {
  unsigned long now = millis();
  if (now - lastDhtRead < 5000) return;   // DHT11: alle 5s reicht
  lastDhtRead = now;
  int chk = DHT11.read(PIN_DHT11); 
  lastClimate.temp = DHT11.temperature; 
  lastClimate.hum = DHT11.humidity; 
}
// ----------------- helpers -------------------------------
static uint8_t clampU8(int v) {
  if (v < 0) return 0;
  if (v > 255) return 255;
  return (uint8_t)v;
}

// Ein Wert x wird auf die Range 0..100 abgebildet
// zB mapF(26.0, 24.0, 30.0) --> 85%
//         30,  24.0, 30.0,) --> 100%
static float getPctInRange(float x, float inMin, float inMax) {
  if ( x > inMax ) x = inMax;
  if ( x < inMin ) x = inMin;
  if ( inMax == inMin ) return 0.0f;  // DIV/0
  float norm =  100.0f * (x - inMin) / (inMax - inMin);
  // Serial.printf("norm=%.2f mapF: x=%.2f in=[%.2f..%.2f] out=[%.2f..%.2f]\n", norm, x, inMin, inMax, 0.0f, 100.0f);
  return norm;
}

void warnton() {
  for (uint16_t i = 0; i < 4; i++) {
    buzzer.playTone(500,1000);
    delay(500);
  }
  buzzer.playTone(0, 0);
}

// ----------------- HumTemp  -----------------
float getTemperature() { return lastClimate.temp; }
float getHuminity()    { return lastClimate.hum; }

// ----------------- RGB LED -----------------
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

// ----------------- gelbe LED -----------------
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

// -------------------- Steuerung für Fenster / Tür und Fan --------------------
void ctrWindow(WindowState state) {
    if (!USE_WINDOW) return;
    int angle = 0;
    switch (state) {
        case WINDOW_OPEN:
           // angle = 176;   // ganz auf
            angle = 200;   // ganz auf
            break;
        case WINDOW_CLOSED:
        default:
            angle = 0;     // ganz zu
            break;
    }
    windowServo.write(angle);
}
// Tür je nach Winkel justieren. 
void ctrDoor(DoorState state) {
    if (!USE_DOOR) return;
    int angle = 0;
    switch (state) {
        case DOOR_OPEN:
            angle = 90;  
            break;
        case DOOR_CLOSED:
        default:
            angle = 0;
            break;
    }
    doorServo.write(angle);
}

void setFanSpeed(uint8_t speed) {
  // lt. tutorial muss INA-INB >=45 sein (gegen uhrzeigersinn, nur eine richtung implementiert)
  if (speed < 45) {
    digitalWrite(PIN_FAN_INA, LOW);
    analogWrite(PIN_FAN_INB, 0);
  } else {
    digitalWrite(PIN_FAN_INA, HIGH);
    analogWrite(PIN_FAN_INB, speed);
  }
  fanSpeed = speed;
}

// nur ein/aus 
void ctrFan(FanState state) {
  switch (state) {
    case FAN_ON:
      setFanSpeed(180);
      break;
    case FAN_OFF:
      setFanSpeed(0);
      break;
    default:
      break;
  }
}

// verwendet lastClimate zur Kontrolle der Geschwindigkeit aber nur bis max. speed 180
void ctrFan() {
  uint8_t fan = 0;
  if ( !(isnan(lastClimate.temp) || isnan(lastClimate.hum)) ) {
    float pct = max( getPctInRange(lastClimate.temp, 24.0f, 30.0f) , getPctInRange(lastClimate.hum , 50.0f, 80.0f) );
    fan = (uint8_t)roundf(pct * 1.8f);
  }
  setFanSpeed(fan);
} 

// Rückgabe der Fan-Geschwindigkeit 
uint8_t getFanSpeed() {
  return fanSpeed;
}




