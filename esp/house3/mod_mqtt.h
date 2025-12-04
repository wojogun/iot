#ifndef MOD_MQTT_H
#define MOD_MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>

extern const char* TOPIC_CMD_PARTY;
extern const char* TOPIC_CMD_STORM;
extern const char* TOPIC_BC_PARTY;
extern const char* TOPIC_BC_STORM;
extern const char* TOPIC_STATUS_HOUSE3;
extern const char* TOPIC_NEXT_PARTY;
extern const char* TOPIC_CURRENT_SONG;

void initMQTT();
void mqttLoop();
PubSubClient& GetMqttClient();

#endif