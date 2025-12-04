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
