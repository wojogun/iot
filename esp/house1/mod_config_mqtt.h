#ifndef MOD_CONFIG_MQTT_H
#define MOD_CONFIG_MQTT_H

#include <Arduino.h>
#include <PubSubClient.h>

typedef void (*MqttCallback)(const String& topic, const String& payload);

void initMqtt();
void mqttLoop();
PubSubClient& getMqttClient();

bool publishMqtt(const String& topic, const String& payload, bool retained = false);
bool subscribeMqtt(const String& topic);
bool isConnectedMqtt();

void registerCallbackMqtt(MqttCallback cb);

#endif