#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <BuzzerESP32.h>

#include "hardware.h"
#include "mod_partylogic.h"
#include "mod_lcd.h"
#include "mod_mqtt.h"
#include "mod_songs.h"

Mode currentMode = MODE_NORMAL;
auto& mqttClient = GetMqttClient();

static unsigned long lastPartyUpdate = 0;
static unsigned long lastStormBlink  = 0;
static bool          stormLedState   = false;
static uint16_t      partyHue        = 0;

// interne Helfer-Funktionen
static void updateEffects();
static void partyLightsStep(unsigned long now);
static void stormBlinkStep(unsigned long now);

// Von MQTT gesetzter Text; wird z.B. bei stopStorm() angezeigt
String nextPartyText;

void initPartyLogic() {
  initHardware();
}

void partyLoop() {
  updateEffects();
}

void updateEffects() {
  unsigned long now = millis();

  if (currentMode == MODE_PARTY) {
    partyLightsStep(now);
  } else if (currentMode == MODE_STORM) {
    stormBlinkStep(now);
  }
}

void partyLightsStep(unsigned long now) {
  const unsigned long interval = 30;
  if (now - lastPartyUpdate < interval) return;
  lastPartyUpdate = now;

  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    int pixelHue = partyHue + (i * 65536L / NEOPIXEL_COUNT);
    uint32_t color = strip.gamma32(strip.ColorHSV(pixelHue));
    strip.setPixelColor(i, color);
  }
  strip.show();
  partyHue += 256;
}

void stormBlinkStep(unsigned long now) {
  const unsigned long interval = 500;
  if (now - lastStormBlink < interval) return;
  lastStormBlink = now;

  stormLedState = !stormLedState;
  digitalWrite(PIN_LED_YELLOW, stormLedState ? HIGH : LOW);
}

void startParty(bool publish) {
  if (currentMode == MODE_STORM) return;
  currentMode = MODE_PARTY;

  ctrWindow(WINDOW_CLOSED);
  ctrDoor(DOOR_CLOSED);
  digitalWrite(PIN_LED_YELLOW, LOW);

  printLcd(	"Party laeuft", "Haus 3", false);
  strip.show();
  playSong(SongId::SONG1);
  //smokeOnTheWater();

  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_PARTY, "STARTED");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "PARTY");
  }
}

void stopParty(bool publish) {
  if (currentMode != MODE_PARTY) return;
  
  playSong(SongId::SONG5);
  //werHatAnDerUhrGedreht();
  currentMode = MODE_NORMAL;

  strip.clear();
  strip.show();

  buzzer.playTone(0, 0);
  ctrWindow(WINDOW_OPEN);
  ctrDoor(DOOR_OPEN);
  printLcd(	"Nächste Party:", nextPartyText, false);
  
  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_PARTY, "STOPPED");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL");
  }
}

void startStorm(bool publish) {
  currentMode = MODE_STORM;

  ctrWindow(WINDOW_CLOSED);
  ctrDoor(DOOR_CLOSED);  strip.clear();
  strip.show();
  buzzer.playTone(0, 0);

  digitalWrite(PIN_LED_YELLOW, HIGH);
  printLcd(	"STURMWARNUNG", "", true);	

  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_STORM, "ON");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "STORM");
  }
}

void stopStorm(bool publish) {
  currentMode = MODE_NORMAL;

  ctrWindow(WINDOW_OPEN);
  ctrDoor(DOOR_OPEN);
  digitalWrite(PIN_LED_YELLOW, LOW);
  printLcd(	"Nächste Party:", nextPartyText, false);

  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_STORM, "OFF");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL");
  }
}