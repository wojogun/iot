// This module manages common functions like gas and storm alarms.

// Includes
#include <Adafruit_NeoPixel.h>
#include <Arduino.h>
#include "driver/ledc.h"
#include "mod_common.h"
#include "mod_mqtt.h"
#include "mod_lcd.h"
#include "mod_door.h"
#include "mod_window.h"
#include "mod_keymgmt.h"

// -------------------- Pinbelegung (KS5009-Standard) --------------------
const uint8_t PIN_LED_YELLOW   = 12;  // gelbe LED am Haus
const uint8_t PIN_BUZZER       = 25;  // Buzzer
const uint8_t PIN_LED_STRIP    = 26;  // SK6812 / NeoPixel

// -------------------- Globale Objekte --------------------
BuzzerESP32 buzzer(PIN_BUZZER);

// RGB LED Strip (NeoPixel / SK6812)
static const uint8_t LED_COUNT = 4;  // Anzahl der Pixel im Strip (RGB)

// Declare NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, PIN_LED_STRIP, NEO_GRB + NEO_KHZ800);

// gelbe LED
static bool     ledBlinkEnabled  = false;
static bool     ledState         = false;
static uint32_t ledBlinkInterval = 500;
static uint32_t ledLastToggle    = 0;

// -------------------- Statusvariablen --------------------
Onoff currentGasStatus = OFF;
Onoff currentStormStatus = OFF;
Onoff currentPartyStatus = OFF;
static bool rfidKeyInitialized = false;  // Track if RFID key was loaded on startup

// Get MQTT client reference
auto& mqttClient = getMqttClient();

// Handle incoming MQTT messages
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
  // Handle retained config topic for RFID key
  else if (topic == TOPIC_CONFIG_RFID_KEY) {
    bool ok = payload.length() ? setRfidKey(payload) : false;
    // Only display message if not the initial startup (skip retained message on connect)
    if (rfidKeyInitialized) {
      printLcd("RFID-Key", ok ? "aktualisiert" : "Fehler", false);
    }
    rfidKeyInitialized = true;
    auto& mqttClient = getMqttClient();
    if (mqttClient.connected()) {
      extern String getRfidKeyName(const String& uid); // ensure function is visible
      String keyName = ok ? getRfidKeyName(payload) : "UPDATE_FAILED";
      mqttClient.publish(TOPIC_CURRENT_RFID_KEY, keyName.c_str());
    }
  }
}

// Gas alarm
void startGas(bool publish) {
  currentGasStatus = ON;
  if (mqttClient.connected()) mqttClient.publish(TOPIC_STATUSGAS_HOUSE2, "ON");
  printWarnings();
  startRgbRedLight(10); // Flowing red cycle along the whole strip
}

// Stop gas alarm
void stopGas(bool publish) {
  currentGasStatus = OFF;
  if (mqttClient.connected()) mqttClient.publish(TOPIC_STATUSGAS_HOUSE2, "OFF");
  printWarnings();
  strip.clear(); // Clear all pixel data
  strip.show(); // Turn OFF all pixels ASAP
}

// Storm alarm
void startStorm(bool publish) {
  currentStormStatus = ON;
  if (publish && mqttClient.connected())  mqttClient.publish(TOPIC_BC_STORM, "ON");
  if            (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE2, "ON");
  closeWindow();
  closeDoor();
  switchLed(true);
  printWarnings();
  // Play buzzer sound
  buzzer.playTone(494, 250); // (0,0) um auszuschalten
  delay(100);
  buzzer.playTone(494, 250);
  delay(100);
  buzzer.playTone(494, 250);
}

// Stop storm alarm
void stopStorm(bool publish) {
  currentStormStatus = OFF;
  if (publish && mqttClient.connected()) mqttClient.publish(TOPIC_BC_STORM, "OFF");
  if            (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE2, "OFF");
  openWindow();
  openDoor();
  buzzer.playTone(0, 0); // Ensure buzzer is off
  switchLed(false);
  printWarnings();
}

// Party mode
void startParty(bool publish) {
  currentPartyStatus = ON;
  if (publish && mqttClient.connected())  mqttClient.publish(TOPIC_BC_PARTY, "ON");
  if            (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSPARTY_HOUSE2, "ON");
  closeWindow();
  printWarnings();
}

// Stop party mode
void stopParty(bool publish) {
  currentPartyStatus = OFF;
  if (publish && mqttClient.connected())  mqttClient.publish(TOPIC_BC_PARTY, "OFF");
  if            (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSPARTY_HOUSE2, "OFF");
  openWindow();
  printWarnings();
}

// Print warnings on LCD
void printWarnings() {
  uint8_t warnMask = (currentStormStatus == ON ? 0b10 : 0) | (currentGasStatus == ON ? 0b01 : 0) | (currentPartyStatus == ON ? 0b100 : 0);
  Serial.println("Warnmask: " + String(warnMask, BIN));
  switch (warnMask) {
    case 0b00:
      printLcd("keine aktuellen", "Resort-Warnungen", false);
      break;
    case 0b10:
      printLcd("STURMWARNUNG", "", false);
      break;
    case 0b01:
      printLcd("GASWARNUNG", "", false);
      break;
    case 0b11:
      printLcd("STURM + GAS", "WARNUNG", false);
      break;
    case 0b100:
      printLcd("WOLFI IST", "ZU LAUT !!!", false);
      break;
  }
}

// Switch yellow LED on or off
void switchLed(bool onoff) {
  ledState = onoff;
  ledBlinkEnabled = false;
  digitalWrite(PIN_LED_YELLOW, (onoff ? HIGH : LOW));
}

// RED light cycle along whole strip. Pass delay time (in ms) between frames.
void startRgbRedLight(int wait) {
  // Display only RED colors on all pixels in the strip.
  for (int j = 0; j < 50; ++j) { // Show red for a short animation (adjust 50 as needed)
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(255, 0, 0)); // Pure red
    }
    strip.show();
    delay(wait);
  }
}

// Initialise common module
void InitCommon() {
  
  // Initialise NeoPixel strip object
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)

  // LED
  pinMode(PIN_LED_YELLOW, OUTPUT);
  digitalWrite(PIN_LED_YELLOW, LOW);

  // Buzzer
  buzzer.setTimbre(30);      // Klangfarbe (Keyestudio-Beispiel)
  buzzer.playTone(0, 0);     // Ensure buzzer is off
  
  // MQTT
  registerCallbackMqtt(handleMqtt);

  // Re-subscribe on connect/reconnect
  registerOnConnectMqtt([](){
    subscribeMqtt(TOPIC_BC_STORM);
    subscribeMqtt(TOPIC_BC_GAS);
    subscribeMqtt(TOPIC_BC_PARTY);
    subscribeMqtt(TOPIC_CONFIG_RFID_KEY); // retained key fetched here too
  });

  // Also subscribe now (covers initial boot where we connected before registering the hook)
  subscribeMqtt(TOPIC_BC_STORM);
  subscribeMqtt(TOPIC_BC_GAS);
  subscribeMqtt(TOPIC_BC_PARTY);
  subscribeMqtt(TOPIC_CONFIG_RFID_KEY);

  auto& mqttClient = getMqttClient();
  mqttClient.publish(TOPIC_STATUS_HOUSE2, "NORMAL");
  mqttClient.publish(TOPIC_STATUSGAS_HOUSE2, "OFF");
  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE2, "OFF");
  mqttClient.publish(TOPIC_STATUSPARTY_HOUSE2, "OFF");
}