#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "config.h"
#include "mod_wifi.h"
#include "mod_mqtt.h"
#include "mod_lcd.h"
// #include "mod_partylogic.h"   // für startParty/startStorm, falls nötig für overall case


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

void mqttLoop() {
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

// static void reconnectMqtt() {
//   while (!mqttClient.connected()) {
//     String clientId = "house_";
//     clientId += String(random(0xffff), HEX);

//     Serial.print("Connecting MQTT as ");
//     Serial.println(clientId);

//     if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
//       Serial.println("MQTT connected");
//       // Subscriptions macht dein Hausmodul
//     } else {
//       Serial.print("failed rc=");
//       Serial.print(mqttClient.state());
//       Serial.println(" retry in 5 seconds");
//       delay(5000);
//     }
//   }
// }

// // MQTT-Topics
// const char* TOPIC_CMD_PARTY     = "resort/house3/party/cmd";
// const char* TOPIC_CMD_STORM     = "resort/house3/storm/cmd";
// const char* TOPIC_BC_PARTY      = "resort/broadcast/party";
// const char* TOPIC_BC_STORM      = "resort/broadcast/storm";
// const char* TOPIC_STATUS_HOUSE3 = "resort/house3/status";
// const char* TOPIC_NEXT_PARTY    = "resort/house3/party/next";
// const char* TOPIC_CURRENT_SONG  = "resort/house3/party/song"; 

// static void mqttCallback(char* topic, byte* payload, unsigned int length);
// static void reconnectMQTT();

// void initMQTT() {
//   //secureClient.setCACert(ROOT_CA);
//   secureClient.setInsecure();
//   mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
//   mqttClient.setCallback(mqttCallback);
// }

// PubSubClient& GetMqttClient() {
//     return mqttClient;
// }

// void mqttLoop() {
//   if (!wiFiIsConnected()) return;    // ohne WLAN kein MQTT
//   if (!mqttClient.connected()) reconnectMQTT();
//   mqttClient.loop();
// }

// void reconnectMQTT() {
//   Serial.print("Attempting MQTT connection... ");

//   String clientId = "Haus3Party";
//   clientId += String(random(0xffff), HEX);

//   if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
//     Serial.println("connected");

//     mqttClient.subscribe(TOPIC_CMD_PARTY);
//     mqttClient.subscribe(TOPIC_CMD_STORM);
//     mqttClient.subscribe(TOPIC_BC_STORM);
//     mqttClient.subscribe(TOPIC_NEXT_PARTY); 
    
//     mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL");
//   } else {
//     Serial.print("failed, rc=");
//     Serial.print(mqttClient.state());
//     Serial.println(" will retry later");
//   }
// }

// void mqttCallback(char* topic, byte* payload, unsigned int length) {
//   String t = String(topic);
//   String msg;
//   for (unsigned int i = 0; i < length; i++) {
//     msg += (char)payload[i];
//   }

//   Serial.print("MQTT in [");
//   Serial.print(t);
//   Serial.print("]: ");
//   Serial.println(msg);

//   // if (t == TOPIC_CMD_PARTY) {
//   //   if (msg == "START") {
//   //     startParty(false);
//   //   } else if (msg == "STOP") {
//   //     stopParty(false);
//   //   }
//   // } else if (t == TOPIC_CMD_STORM || t == TOPIC_BC_STORM) {
//   //   if (msg == "ON") {
//   //     startStorm(false);
//   //   } else if (msg == "OFF") {
//   //     stopStorm(false);
//   //   }
//   // } else if (t == TOPIC_NEXT_PARTY) {
//   //   nextPartyText = msg;   // String aus der MQTT-Nachricht
//   //   // Nur in NORMAL-Modus sofort anzeigen, bei Party/Storm bleibt Text
//   //   if (currentMode == MODE_NORMAL) {
// 	//   printLcd("Next: ", nextPartyText, false); 
//   //   }
//   }




