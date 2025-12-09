#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <BuzzerESP32.h>

#include "hardware.h"
#include "config.h"
#include "mod_mqtt.h"
#include "mod_partylogic.h"
#include "mod_lcd.h"
#include "mod_songs.h"

Mode currentMode = MODE_NORMAL;
auto& mqttClient = getMqttClient();

static unsigned long lastPartyUpdate = 0;
static unsigned long lastStormBlink  = 0;
static bool          stormLedState   = false;
static uint16_t      partyHue        = 0;

void handleMqtt(const String& topic, const String& payload) {
  Serial.print("MQTT in [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(payload);

  if (topic == TOPIC_CMD_PARTY) {
    if (payload == "START") startParty(false);
    else if (payload == "STOP") stopParty(false);
  }

  else if (topic == TOPIC_CMD_STORM || topic == TOPIC_BC_STORM) {
    if (payload == "ON") startStorm(false);
    else if (payload == "OFF") stopStorm(false);
  }

  else if (topic == TOPIC_CMD_SONG) {
    SongId song = SongId::NONE;
    if (payload == "1") {
        song = SongId::SONG1;
    } else if (payload == "2") {
        song = SongId::SONG2;
    } else if (payload == "3") {
        song = SongId::SONG3;
    } else if (payload == "4") {
        song = SongId::SONG4;
    } else if (payload == "5") {
        song = SongId::SONG5;
    } else {
        song = SongId::NONE;
    }
    if (song == SongId::NONE) {
      // Song stoppen / NONE setzen
      sendMqttSongName(SongId::NONE);
      Serial.println("[MQTT] Song STOP");
    } else {
      playSong(song);
      Serial.print("[MQTT] Song START: ");
      Serial.println(songName(song));
    }
  } else if (topic == TOPIC_CMD_NEXT) {
    nextPartyText = payload;
    if (currentMode == MODE_NORMAL) {
      printLcd("Next: ", nextPartyText, false); 
      Serial.println("ok");	
    }
  }
}

// interne Helfer-Funktionen
static void updateEffects();
static void partyLightsStep(unsigned long now);
static void stormBlinkStep(unsigned long now);

// Von MQTT gesetzter Text; wird z.B. bei stopStorm() angezeigt
String nextPartyText;

void initParty() {
  initHardware();
  registerCallbackMqtt(handleMqtt);

  subscribeMqtt(TOPIC_BC_STORM);
  //subscribeMqtt(TOPIC_BC_PARTY); wird lokal behandelt
  subscribeMqtt(TOPIC_CMD_PARTY);
  subscribeMqtt(TOPIC_CMD_STORM);
  subscribeMqtt(TOPIC_CMD_SONG);
  subscribeMqtt(TOPIC_CMD_NEXT);
  // TOPIC_STATUS_HOUSE3  = "resort/house3/status";      --> publish only
  // TOPIC_CURRENT_SONG   = "resort/house3/party/song";  --> publish only

  Serial.println("Partylogic subscribed all topics");
}

void loopParty() {
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
  ctrFan(FAN_ON);
  digitalWrite(PIN_LED_YELLOW, LOW);

  printLcd(	"Party laeuft", "Haus 3", false);
  strip.show();
  playSong(SongId::SONG3);   //smokeOnTheWater();

  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_PARTY, "START");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "PARTY");
  }
}

void stopParty(bool publish) {
  if (currentMode != MODE_PARTY) return;
  
  ctrFan(FAN_OFF);
  currentMode = MODE_NORMAL;

  strip.clear();
  strip.show();

  playSong(SongId::SONG5);
  buzzer.playTone(0, 0);

  ctrWindow(WINDOW_OPEN);
  ctrDoor(DOOR_OPEN);
  printLcd(	"Next:", nextPartyText, false);
  
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
  printLcd(	"Next Party:", nextPartyText, false);

  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_STORM, "OFF");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL");
  }
}