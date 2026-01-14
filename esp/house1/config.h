#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==== WIFI ====
struct WiFiEntry {
  const char* ssid;
  const char* pass;
};

// Reihenfolge = Priorität (Index 0 = MASTER)
extern const WiFiEntry WIFI_LIST[];
extern const uint8_t   WIFI_LIST_COUNT;

// ==== MQTT ====
extern const char* MQTT_SERVER;
extern const int   MQTT_PORT;
extern const char* MQTT_USER;
extern const char* MQTT_PASSWORD;

// ===== TOPICS HAUS 1 =====
// Status Topics:
extern const char* TOPIC_STATUS_HOUSE1;              // "resort/house1/status"
extern const char* TOPIC_STATUS_GAS_HOUSE1;          // "resort/house1/status/gas";  
extern const char* TOPIC_STATUS_STORM_HOUSE1;        // "resort/house1/status/storm";  
extern const char* TOPIC_STATUS_PARTY_HOUSE1;        // "resort/house1/status/party";   

extern const char* TOPIC_STATUS_TEMP;               // "resort/house1/status/temp"
extern const char* TOPIC_STATUS_HUM;                // "resort/house1/status/hum"
extern const char* TOPIC_STATUS_STEAM;              // "resort/house1/status/steam"
extern const char* TOPIC_STATUS_WIND;               // "resort/house1/status/wind"
extern const char* TOPIC_STATUS_WEATHER_VALUES;     // "resort/house1/status/weathervalues"

// Broadcast Haus 1 an alle
extern const char* TOPIC_BC_STORM;            // "resort/bc/storm"
// Broadcast alle
extern const char* TOPIC_BC_PARTY;            // "resort/bc/party"
extern const char* TOPIC_BC_GAS;              // "resort/bc/gas"      


// Simulation / Command Topics
extern const char* TOPIC_STORM_CMD;           // "resort/house1/storm/cmd"
extern const char* TOPIC_FAN_CMD;             // "resort/house1/fan/cmd"
extern const char* TOPIC_SIM_ENABLED;         // "resort/house1/simulation/enabled"
extern const char* TOPIC_SIM_WIND;            // "resort/house1/simulation/wind"
extern const char* TOPIC_SIM_STEAM;           // "resort/house1/simulation/steam"
extern const char* TOPIC_SIM_TEMP;            // "resort/house1/simulation/temp"


// TEST/DEBUG Buttons
extern const char* TOPIC_BUTTON_BTN1;    // "resort/house1/button/btn1"
extern const char* TOPIC_BUTTON_BTN2;    // "resort/house1/button/btn2"

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
