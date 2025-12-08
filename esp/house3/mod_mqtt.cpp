#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include "config.h"
#include "mod_wifi.h"
#include "mod_mqtt.h"
#include "mod_partylogic.h"   // für startParty/startStorm, falls nötig
#include "mod_lcd.h"

// TLS-Client + MQTT-Client
static WiFiClientSecure secureClient;
static PubSubClient mqttClient(secureClient);

// MQTT-Topics
const char* TOPIC_CMD_PARTY     = "resort/house3/party/cmd";
const char* TOPIC_CMD_STORM     = "resort/house3/storm/cmd";
const char* TOPIC_BC_PARTY      = "resort/broadcast/party";
const char* TOPIC_BC_STORM      = "resort/broadcast/storm";
const char* TOPIC_STATUS_HOUSE3 = "resort/house3/status";
const char* TOPIC_NEXT_PARTY    = "resort/house3/party/next";
const char* TOPIC_CURRENT_SONG  = "resort/house3/party/song"; 

static void mqttCallback(char* topic, byte* payload, unsigned int length);
static void reconnectMQTT();

void initMQTT() {
  //secureClient.setCACert(ROOT_CA);
  secureClient.setInsecure();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
}

PubSubClient& GetMqttClient() {
    return mqttClient;
}

void mqttLoop() {
  if (!wiFiIsConnected()) return;    // ohne WLAN kein MQTT
  if (!mqttClient.connected()) reconnectMQTT();
  mqttClient.loop();
}

void reconnectMQTT() {
  Serial.print("Attempting MQTT connection... ");

  String clientId = "Haus3Party";
  clientId += String(random(0xffff), HEX);

  if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
    Serial.println("connected");

    mqttClient.subscribe(TOPIC_CMD_PARTY);
    mqttClient.subscribe(TOPIC_CMD_STORM);
    mqttClient.subscribe(TOPIC_BC_STORM);
    mqttClient.subscribe(TOPIC_NEXT_PARTY); 
    
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL");
  } else {
    Serial.print("failed, rc=");
    Serial.print(mqttClient.state());
    Serial.println(" will retry later");
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String t = String(topic);
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("MQTT in [");
  Serial.print(t);
  Serial.print("]: ");
  Serial.print(msg);

  if (t == TOPIC_CMD_PARTY) {
    if (msg == "START") {
      startParty(false);
	  Serial.println("ok");	
    } else if (msg == "STOP") {
      stopParty(false);
	  Serial.println("ok");	
    } else {
	  Serial.println("nicht gefunden!");	
	}
  } else if (t == TOPIC_CMD_STORM || t == TOPIC_BC_STORM) {
    if (msg == "ON") {
      startStorm(false);
	  Serial.println("ok");	
    } else if (msg == "OFF") {
      stopStorm(false);
	  Serial.println("ok");	
    } else {
	  Serial.println("nicht gefunden!");	
	}
  } else if (t == TOPIC_NEXT_PARTY) {
    nextPartyText = msg;   // String aus der MQTT-Nachricht
    // Nur in NORMAL-Modus sofort anzeigen, bei Party/Storm bleibt Text
    if (currentMode == MODE_NORMAL) {
	  printLcd("Next: ", nextPartyText, false); 
	  Serial.println("ok");	
    }
  }
}





