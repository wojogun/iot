#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
// #include <IPAddress.h>

// ==== WIFI ====
struct WiFiEntry {
  const char* ssid;
  const char* pass;
};

// Reihenfolge = Priorität (Index 0 = MASTER)
extern const WiFiEntry WIFI_LIST[];
extern const uint8_t   WIFI_LIST_COUNT;
// extern const char* WIFI_SSID;
// extern const char* WIFI_PASSWORD;

// ==== MQTT ====
extern const char* MQTT_SERVER;
extern const int   MQTT_PORT;
extern const char* MQTT_USER;
extern const char* MQTT_PASSWORD;

// Haus 1 – Topics
extern const char* TOPIC_STATUS_HOUSE1;     // "resort/house1/status"

extern const char* TOPIC_WEATHER_VALUES;    // "resort/house1/weather"
extern const char* TOPIC_TEMP;              // "resort/house1/weather/temp"
extern const char* TOPIC_HUM;               // "resort/house1/weather/hum"
extern const char* TOPIC_STEAM;             // "resort/house1/weather/steam"
extern const char* TOPIC_WIND;              // "resort/house1/weather/wind"

extern const char* TOPIC_STORM_ALERT;          // "resort/house1/storm/alert" (0/1)
extern const char* TOPIC_BC_STORM;          // z.B. "resort/bc/storm"

// Manuelle Steuerung durch/in Node-RED
extern const char* TOPIC_STORM_CMD;         // "resort/house1/cmd/storm"  (AUTO/ON/OFF)
extern const char* TOPIC_WIND_CMD;          // "resort/house1/cmd/wind"   (0–100)

// TODO
extern const char* TOPIC_TEMP_CMD;          // "resort/house1/cmd/temp"   (-10–45)
extern const char* TOPIC_STEAM_CMD;          // "resort/house1/cmd/steam"   (DRY/RAIN/FOG)

// Testen: Command, um Sturm/Gas nur zu testen
extern const char* TOPIC_CMD_STORM_H1;     // "resort/house1/cmd/storm"

extern const char* TOPIC_MOTION;            // "resort/house1/motion" (Bewgung - Perimeterschutz)



// Haus 3 CMD, Broadcast etc (vielleicht später wieder verwenden bei Overall Case)
// extern const char* TOPIC_STATUS_HOUSE3;
// extern const char* TOPIC_CMD_PARTY;
// extern const char* TOPIC_CMD_STORM;
// extern const char* TOPIC_CMD_SONG;
// extern const char* TOPIC_CMD_NEXT;
// extern const char* TOPIC_BC_STORM;
// extern const char* TOPIC_BC_PARTY;
// extern const char* TOPIC_BC_GAS;
// extern const char* TOPIC_CURRENT_SONG;
// extern const char* TOPIC_SONGLIST;

// ==== STATIC IP / DHCP ====
extern const bool USE_STATIC_IP;
// extern IPAddress local_IP;
// extern IPAddress gateway;
// extern IPAddress subnet;
// extern IPAddress dns;

// ==== mDNS Name ====
extern const char* MDNS_NAME;

// ==== ROOT CA ====
// extern const char ROOT_CA[] PROGMEM;

#endif
