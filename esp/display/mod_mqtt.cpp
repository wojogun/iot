#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "config.h"
#include "mod_wifi.h"
#include "mod_mqtt.h"

// TLS-Client + MQTT-Client
static WiFiClientSecure secureClient;
static PubSubClient mqttClient(secureClient);

static MqttCallback registeredCallback = nullptr; // handler für das jeweilige hausc:\Users\wojog\OneDrive\uni\FH_Master\3_IoT\smarthome\esp\house3\mod_mqtttopics.h

void registerCallbackMqtt(MqttCallback cb) { registeredCallback = cb; }

static void internalCallback(char* topic, byte* payload, unsigned int length) {
  if (!registeredCallback) return;

  String t(topic);
  String msg;
  msg.reserve(length);
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  registeredCallback(t, msg);
}

PubSubClient& getMqttClient() { return mqttClient; }

static void reconnectMqtt() {
  while (!mqttClient.connected()) {
    String clientId = "house_";
    clientId += String(random(0xffff), HEX);

    Serial.print("Connecting MQTT as ");
    Serial.println(clientId);

    // WICHTIG: hier Username/Password mitgeben
    if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("MQTT connected");
      // Subscriptions macht dein Hausmodul
    } else {
      Serial.print("failed rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retry in 5 seconds");
      delay(5000);
    }
  }
}

void initMqtt() {
  //secureClient.setCACert(ROOT_CA);
  secureClient.setInsecure();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(internalCallback);
  reconnectMqtt(); 
}

void loopMqtt() {
  if (!wiFiIsConnected()) return;    // ohne WLAN kein MQTT
  if (!mqttClient.connected()) {
    reconnectMqtt();
  }
  mqttClient.loop();
}

bool publishMqtt(const String& topic, const String& payload, bool retained) {
  if (!mqttClient.connected()) return false;
  return mqttClient.publish(topic.c_str(), payload.c_str(), retained);
}

bool subscribeMqtt(const String& topic) {
  if (!mqttClient.connected()) {
    Serial.print("subscribeMqtt ignored (not connected): ");
    Serial.println(topic);
    return false;
  }

  bool ok = mqttClient.subscribe(topic.c_str());

  Serial.print("subscribeMqtt ");
  Serial.print(topic);
  Serial.print(" → ");
  Serial.println(ok ? "OK" : "FAILED");

  return ok;
}

bool isConnectedMqtt() {
  return mqttClient.connected();
}