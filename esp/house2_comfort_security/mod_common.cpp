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

// -------------------- Pinbelegung (KS5009-Standard) --------------------
const uint8_t PIN_LED_YELLOW   = 12;  // gelbe LED am Haus
const uint8_t PIN_MOTION       = 14;
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
}

// Gas alarm
void startGas(bool publish) {
  currentGasStatus = ON;
  if (mqttClient.connected()) mqttClient.publish(TOPIC_STATUSGAS_HOUSE2, "ON");
  printWarnings();
  startRgbRainbow(10); // Flowing rainbow cycle along the whole strip
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
      printLcd("keine aktuellen", "Warnungen", false);
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

// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void startRgbRainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
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
  subscribeMqtt(TOPIC_BC_STORM);
  subscribeMqtt(TOPIC_BC_GAS);
  subscribeMqtt(TOPIC_BC_PARTY);
  mqttClient.publish(TOPIC_STATUS_HOUSE2, "NORMAL");
  mqttClient.publish(TOPIC_STATUSGAS_HOUSE2, "OFF");
  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE2, "OFF");
  mqttClient.publish(TOPIC_STATUSPARTY_HOUSE2, "OFF");
}