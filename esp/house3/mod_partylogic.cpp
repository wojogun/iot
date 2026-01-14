#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <BuzzerESP32.h>

#include "hardware.h"
#include "config.h"
#include "mod_mqtt.h"
#include "mod_partylogic.h"
#include "mod_lcd.h"
#include "mod_rfid.h"
#include "mod_songs.h"
#include "mod_button.h"
#include "mod_motion.h"

Mode currentMode = MODE_NORMAL;
Onoff currentGasStatus = OFF;
Onoff currentStormStatus = OFF;

auto& mqttClient = getMqttClient();

static unsigned long lastPartyUpdate = 0;
static unsigned long lastStormBlink  = 0;
static bool          stormLedState   = false;
static uint16_t      partyHue        = 0;

// wann gelüftet werden muss
static bool fanOn = false;
static const float FAN_ON_TEMP  = 25.0;
static const float FAN_OFF_TEMP = 24.0;

float oldTemp = NAN;
float oldHum = NAN;

void handleMqtt(const String& topic, const String& payload) {
  Serial.print("MQTT in [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(payload);

  if (topic == TOPIC_CMD_PARTY || topic == TOPIC_BC_PARTY) {
    controlParty( (payload == "PARTY") ? MODE_PARTY : MODE_NORMAL, false);
  } 
  else if (topic == TOPIC_CMD_STORM || topic == TOPIC_BC_STORM) {
    if (payload == "ON") startStorm(false);
    else if (payload == "OFF") stopStorm(false);
    else Serial.print("payload unbekannt:" + payload);
  } 
  else if (topic == TOPIC_BC_GAS ) {
    if (payload == "ON")  startGas(false);
    else if (payload == "OFF") stopGas(false);
    else Serial.print("payload unbekannt:" + payload);
  }
  else if (topic == TOPIC_CMD_SONG) {
    playSong(payload.toInt());
  }
  else if (topic == TOPIC_CMD_NEXT) {
    nextPartyText = payload;
    nextPartyText.replace(", ", " ");

    Serial.print("mode=");
    Serial.print(currentMode);
    Serial.print("  nextparty:");
    Serial.println(nextPartyText);
    if (currentMode == MODE_NORMAL) {
      if (nextPartyText=="") nextPartyText = "keine Buchung";
      printLcd("Next: ", nextPartyText, false); 
      Serial.println("ok");	
    }
  }
}

// interne Helfer-Funktionen
static void updateEffects();
static void partyLightsStep(unsigned long now);

// Von MQTT gesetzter Text; wird z.B. bei stopStorm() angezeigt
String nextPartyText;

void initParty() {
  registerCallbackMqtt(handleMqtt);
  registerRfidCallback(handleRfidSong);
  registerMotionCallback(onMotion);

  subscribeMqtt(TOPIC_BC_STORM);
  subscribeMqtt(TOPIC_BC_GAS);
  //subscribeMqtt(TOPIC_BC_PARTY);  //wird lokal behandelt, daher nur Haus 1,2,4
  //subscribeMqtt(TOPIC_CMD_STORM); // nur Haus 1
  subscribeMqtt(TOPIC_CMD_PARTY);   // nur Haus3
  subscribeMqtt(TOPIC_CMD_SONG);    // nur Haus3
  subscribeMqtt(TOPIC_CMD_NEXT);    // nur Haus3
  // TOPIC_STATUS_HOUSE3  = "resort/house3/status";      --> publish only
  // TOPIC_CURRENT_SONG   = "resort/house3/party/song";  --> publish only
  Serial.println("Partylogic subscribed all topics");
  publishSongList();
  mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL", true);
  mqttClient.publish(TOPIC_STATUSGAS_HOUSE3, "OFF", false);
  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE3, "OFF", false);
  mqttClient.publish(TOPIC_STATUSFAN_HOUSE3, "0", false);
}

void loopParty() {
  unsigned long now = millis();
  // Buttons abfragen (Flanken-Erkennung)
  ButtonEvent ev1 = updateButton(btn1, now);
    if (ev1 == BUTTON_LONG) controlParty(MODE_PARTY,true);
  ButtonEvent ev2 = updateButton(btn2, now);
    if (ev2 == BUTTON_LONG) controlParty(MODE_NORMAL,true);

  if (currentMode == MODE_PARTY) {
    // Lichtorgel nur wenn keine Gaswarnung
    if (currentGasStatus==OFF) partyLightsStep(now);

    // wird es zu heiß oder zu feucht -> Ventilator aktivieren
    float t = getTemperature();
    float h = getHuminity();
    if ( oldTemp != t || oldHum != h) {
      oldTemp = t;
      oldHum = h;
      ctrFan();
      if (mqttClient.connected()) {
        mqttClient.publish(TOPIC_STATUSTEMP_HOUSE3, String(t,1).c_str(), false); 
        mqttClient.publish(TOPIC_STATUSHUM_HOUSE3, String(h,1).c_str(), false);
        mqttClient.publish(TOPIC_STATUSFAN_HOUSE3, String(getFanSpeed()).c_str(), false);
      }
    }
  } 

  // kein echter einsatz nur zum testen
  //if (motionRising()) Serial.println("Rising-Event");
}

void onMotion(bool active) {
  //Serial.println( active ? "Bewegung erkannt" : "Keine Bewegung mehr");
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

// ============= EVENTSTEUERUNG ======================
void startGas(bool publish) {
  currentGasStatus = ON;
  if (mqttClient.connected()) mqttClient.publish(TOPIC_STATUSGAS_HOUSE3, "ON");
  strip.show();
  printWarnings();
  warnton();
}
void stopGas(bool publish) {
  currentGasStatus = OFF;
  if (mqttClient.connected()) mqttClient.publish(TOPIC_STATUSGAS_HOUSE3, "OFF");
  strip.show();
  switchLed(false);
  printWarnings();
}

void startStorm(bool publish) {
  currentStormStatus = ON;
  if (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE3, "ON", true);
  ctrWindow(WINDOW_CLOSED);
  ctrDoor(DOOR_CLOSED);  strip.clear();
  strip.show();
  controlParty(MODE_NORMAL,false);  // Party beenden
  printWarnings();
  warnton();
}
void stopStorm(bool publish) {
  currentStormStatus = OFF;
  if (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE3, "OFF", true);
  ctrWindow(WINDOW_OPEN);
  ctrDoor(DOOR_OPEN);
  switchLed(false);
  printWarnings();
}

void printWarnings() {
  uint8_t warnMask = (currentStormStatus == ON ? 0b10 : 0) | (currentGasStatus == ON ? 0b01 : 0);
  switch (warnMask) {
    case 0b00:
      printLcd("Next:", (nextPartyText == "" ? "keine Buchung" : nextPartyText ), false);
      switchLed(false);
      rgbOff();
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
  }
}

void controlParty( Mode mode, bool publish ) {
  Serial.print("controlParty ");
  Serial.print(mode);
  if ( currentStormStatus == ON && mode == MODE_PARTY) return;
  if ( currentMode == mode ) return;

  if ( mode == MODE_PARTY ) {
    if (mqttClient.connected()) {
      mqttClient.publish(TOPIC_BC_PARTY, "PARTY", true);
      mqttClient.publish(TOPIC_STATUS_HOUSE3, "PARTY", true);
    }
    ctrWindow(WINDOW_CLOSED);
    ctrDoor(DOOR_CLOSED);
    //ctrFan(FAN_ON);
    printLcd(	"Party laeuft", "Haus 3", false);
    strip.show();
    playSong(3);   //smokeOnTheWater();
    currentMode = MODE_PARTY;
  } else if ( mode == MODE_NORMAL ) {
    if (mqttClient.connected()) {
      mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL", true);
      mqttClient.publish(TOPIC_BC_PARTY, "NORMAL", true);
    }
    ctrWindow(WINDOW_OPEN);
    ctrDoor(DOOR_OPEN);
    //ctrFan(FAN_OFF);
    printLcd(	"Next:", nextPartyText, false);    
    playSong(5);
    strip.clear();
    strip.show();
    buzzer.playTone(0, 0);
    currentMode = MODE_NORMAL;
  }
}
