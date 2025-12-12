#ifndef MOD_MQTT_H
#define MOD_MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>

typedef void (*MqttCallback)(const String& topic, const String& payload);

void initMqtt();
void mqttLoop();
PubSubClient& GetMqttClient();

bool publishMqtt(const String& topic, const String& payload, bool retained = false);
bool subscribeMqtt(const String& topic);
bool isConnectedMqtt();

void registerCallbackMqtt(MqttCallback cb);

#endif



// extern const char* TOPIC_CMD_PARTY;
// extern const char* TOPIC_CMD_STORM;
// extern const char* TOPIC_BC_PARTY;
// extern const char* TOPIC_BC_STORM;
// extern const char* TOPIC_STATUS_HOUSE3;
// extern const char* TOPIC_NEXT_PARTY;
// extern const char* TOPIC_CURRENT_SONG;