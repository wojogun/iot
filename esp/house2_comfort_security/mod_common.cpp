// This module manages common functions like gas and storm alarms.

#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "driver/ledc.h"

#include "mod_common.h"
#include "mod_mqtt.h"
#include "mod_lcd.h"
#include "mod_door.h"
#include "mod_window.h"

// -------------------- Pinbelegung (KS5009-Standard) --------------------
const uint8_t NEOPIXEL_COUNT   =  4;
const uint8_t PIN_LED_YELLOW   = 12;  // gelbe LED am Haus
const uint8_t PIN_MOTION       = 14;
const uint8_t PIN_BTN2         = 16;
const uint8_t PIN_BUZZER       = 25;  // Buzzer
const uint8_t PIN_LED_STRIP    = 26;  // SK6812 / NeoPixel
const uint8_t PIN_BTN1         = 27;

// -------------------- Globale Objekte --------------------
BuzzerESP32 buzzer(PIN_BUZZER);

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

Onoff currentGasStatus = OFF;
Onoff currentStormStatus = OFF;
Onoff currentPartyStatus = OFF;

auto& mqttClient = getMqttClient();

void handleMqtt(const String& topic, const String& payload) {
  Serial.print("MQTT in [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(payload);

  if (topic == TOPIC_BC_PARTY) {
    if (payload == "ON") startParty(false);
    else if (payload == "OFF") stopParty(false);
    else Serial.print("payload unbekannt:" + payload);
  } 
  else if (topic == TOPIC_BC_STORM) {
    if (payload == "ON") startStorm(false);
    else if (payload == "OFF") stopStorm(false);
    else Serial.print("payload unbekannt:" + payload);
  } 
  else if (topic == TOPIC_BC_GAS ) {
    if (payload == "ON")  startGas(false);
    else if (payload == "OFF") stopGas(false);
    else Serial.print("payload unbekannt:" + payload);
  }
}

void startGas(bool publish) {
  currentGasStatus = ON;
  // TO DO: RED LED BLINK
  printWarnings();
  if (mqttClient.connected()) mqttClient.publish(TOPIC_STATUSGAS_HOUSE2, "ON");
}
void stopGas(bool publish) {
  currentGasStatus = OFF;
  //switchLed(false);
  printWarnings();
  if (mqttClient.connected()) mqttClient.publish(TOPIC_STATUSGAS_HOUSE2, "OFF");
}

void startStorm(bool publish) {
  currentStormStatus = ON;
  closeWindow();
  closeDoor();
  //buzzer.playTone(0, 0); // temporär ausgeschaltet
  switchLed(true);
  printWarnings();
  if (publish && mqttClient.connected())  mqttClient.publish(TOPIC_BC_STORM, "ON");
  if            (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE2, "ON");
}
void stopStorm(bool publish) {
  currentStormStatus = OFF;
  openWindow();
  openDoor();
  switchLed(false);
  printWarnings();
  if (publish && mqttClient.connected()) mqttClient.publish(TOPIC_BC_STORM, "OFF");
  if            (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE2, "OFF");
}

void startParty(bool publish) {
  currentPartyStatus = ON;
  closeWindow();
  printWarnings();
  if (publish && mqttClient.connected())  mqttClient.publish(TOPIC_BC_PARTY, "ON");
  if            (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSPARTY_HOUSE2, "ON");
}

void stopParty(bool publish) {
  currentPartyStatus = OFF;
  openWindow();
  printWarnings();
  if (publish && mqttClient.connected())  mqttClient.publish(TOPIC_BC_PARTY, "OFF");
  if            (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSPARTY_HOUSE2, "OFF");
}

void printWarnings() {
  uint8_t warnMask = (currentStormStatus == ON ? 0b10 : 0) | (currentGasStatus == ON ? 0b01 : 0) | (currentPartyStatus == ON ? 0b100 : 0);
  Serial.println("Warnmask: " + String(warnMask, BIN));
  switch (warnMask) {
    case 0b00:
      printLcd("keine aktuellen", "Warnungen", false);
      //switchLed(false);
      //rgbBlink(255,0,0);
      break;
    case 0b10:
      printLcd("STURMWARNUNG", "", false);
      blinkLed();
      rgbOff();
      break;
    case 0b01:
      printLcd("GASWARNUNG", "", false);
      switchLed(false);
      rgbBlink(255,0,0);
      break;
    case 0b11:
      printLcd("STURM + GAS", "WARNUNG", false);
      rgbBlink(255,0,0);
      blinkLed();
      break;
    case 0b100:
      printLcd("WOLFI IST", "ZU LAUT !!!", false);
      switchLed(false);
      rgbBlink(255,0,0);
      break;
  }
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

void InitCommon() {
  // LED
  pinMode(PIN_LED_YELLOW, OUTPUT);
  digitalWrite(PIN_LED_YELLOW, LOW);
  initRgb();

  // Buzzer
  buzzer.setTimbre(30);      // Klangfarbe (Keyestudio-Beispiel)
  buzzer.playTone(0, 0);     // sicherstellen, dass er aus ist

  // Buttons
  pinMode(PIN_BTN1, INPUT_PULLUP);
  pinMode(PIN_BTN2, INPUT_PULLUP);
  
  // MQTT
  registerCallbackMqtt(handleMqtt);
  subscribeMqtt(TOPIC_BC_STORM);
  subscribeMqtt(TOPIC_BC_GAS);
  subscribeMqtt(TOPIC_BC_PARTY);
  mqttClient.publish(TOPIC_STATUS_HOUSE2, "NORMAL");
  mqttClient.publish(TOPIC_STATUSGAS_HOUSE2, "OFF");
  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE2, "OFF");
  mqttClient.publish(TOPIC_STATUSPARTY_HOUSE2, "OFF");
}