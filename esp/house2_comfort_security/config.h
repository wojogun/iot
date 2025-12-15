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
//extern const uint8_t   WIFI_LIST_COUNT;

// ==== MQTT ====
extern const char* MQTT_SERVER;
extern const int   MQTT_PORT;
extern const char* MQTT_USER;
extern const char* MQTT_PASSWORD;

// ==== MQTT TOPICS ====
extern const char* TOPIC_STATUS_HOUSE2; // Topic für die Belegung Haus 2
extern const char* TOPIC_STATUS_WINDOW; // Topic für den Fensterstatus
extern const char* TOPIC_STATUS_DOOR;   // Topic für den Türstatus
extern const char* TOPIC_STATUSGAS_HOUSE2; // Topic für den Gasalarmstatus Haus 2
extern const char* TOPIC_STATUSSTORM_HOUSE2; // Topic für den Sturmalarmstatus Haus 2
extern const char* TOPIC_STATUSPARTY_HOUSE2; // Topic für den Partymodusstatus Haus 2

extern const char* TOPIC_BC_STORM; // Topic für Sturmalarm-Broadcast
extern const char* TOPIC_BC_PARTY; // Topic für Partymodus-Broadcast
extern const char* TOPIC_BC_GAS; // Topic für Gasalarm-Broadcast

// ==== STATIC IP / DHCP ====
extern const bool USE_STATIC_IP;
// extern IPAddress local_IP;
// extern IPAddress gateway;
// extern IPAddress subnet;
// extern IPAddress dns;

// ==== mDNS Name ====
extern const char* MDNS_NAME;

// ==== RFID ====
extern const char* RFID_KEY;

#endif
