#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <IPAddress.h>
 
// ==== WIFI ====
// WLAN-Liste
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

extern const char* TOPIC_CMD_PARTY;
extern const char* TOPIC_CMD_STORM;
extern const char* TOPIC_CMD_SONG;
extern const char* TOPIC_CMD_NEXT;
extern const char* TOPIC_BC_STORM;
extern const char* TOPIC_BC_PARTY;
extern const char* TOPIC_CURRENT_SONG;
extern const char* TOPIC_STATUS_STORM;
extern const char* TOPIC_STATUS_HOUSE3;

// ==== STATIC IP / DHCP ====
extern const bool USE_STATIC_IP;
extern IPAddress local_IP;
extern IPAddress gateway;
extern IPAddress subnet;
extern IPAddress dns;

// ==== mDNS Name ====
extern const char* MDNS_NAME;

// ==== ROOT CA ====
extern const char ROOT_CA[] PROGMEM;

#endif
