#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <IPAddress.h>

// ==== WIFI ====
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// ==== MQTT ====
extern const char* MQTT_SERVER;
extern const int   MQTT_PORT;
extern const char* MQTT_USER;
extern const char* MQTT_PASSWORD;

// ==== MQTT TOPICS ====
extern const char* TOPIC_STATUS_HOUSE4; // Topic für die Belegung des Hauses 4
extern const char* TOPIC_STATUS_WINDOW; // Topic für den Fensterstatus
extern const char* TOPIC_STATUS_DOOR;   // Topic für den Türstatus
//extern const char* TOPIC_CMD_PARTY;
//extern const char* TOPIC_CMD_STORM;
// extern const char* TOPIC_CMD_SONG;
// extern const char* TOPIC_CMD_NEXT;
extern const char* TOPIC_BC_STORM;
extern const char* TOPIC_BC_PARTY;
extern const char* TOPIC_BC_GAS;
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
//extern const char ROOT_CA[] PROGMEM;

// ==== RFID ====
extern const char* RFID_KEY;

#endif
