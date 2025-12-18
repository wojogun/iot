#ifndef COMMON_H
#define COMMON_H

// -------------------- Includes --------------------
#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <BuzzerESP32.h>

// -------------------- Enumerations --------------------
enum Onoff {
  ON,
  OFF
};

// -------------------- Pinbelegung (KS5009) --------------------
extern const uint8_t PIN_LED_YELLOW;
extern const uint8_t PIN_LED_STRIP;
extern const uint8_t PIN_BUZZER;

// -------------------- MQTT Topics --------------------
extern const char* TOPIC_STATUS_HOUSE2;
extern const char* TOPIC_STATUSGAS_HOUSE2;
extern const char* TOPIC_STATUSSTORM_HOUSE2;
extern const char* TOPIC_STATUSPARTY_HOUSE2;
extern const char* TOPIC_BC_STORM;
extern const char* TOPIC_BC_PARTY;
extern const char* TOPIC_BC_GAS;
extern const char* TOPIC_CONFIG_RFID_KEY;
extern const char* TOPIC_STATUS_RFID_KEY;

// -------------------- Globale Hardware-Objekte --------------------
extern Adafruit_NeoPixel strip;  // SK6812 / NeoPixel
extern BuzzerESP32 buzzer;

void InitCommon();
void handleMqtt(const String& topic, const String& payload);

void switchLed(bool onoff);
void startRgbRedLight(int wait);

void startGas(bool publish);
void stopGas(bool publish);
void startStorm(bool publish);
void stopStorm(bool publish);
void startParty(bool publish);
void stopParty(bool publish);
void printWarnings();

#endif