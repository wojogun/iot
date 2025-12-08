#include <WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Lexlan";
const char* password = "gekajo67";

const char* mqtt_server = "broker.hivemq.com";  // 🔥 PUBLIC BROKER!
const int mqtt_port = 1883;                     // Standard Port!
const char* mqtt_topic = "esp32/sensor";

WiFiClient espClient;         // NORMAL (kein TLS!)
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("\n🚀 === PUBLIC BROKER (Uni-Firewall OK) ===");

  // 1. WLAN
  Serial.println("📡 WLAN...");
  connectWiFi();
  
  // 2. PUBLIC MQTT
  Serial.println("\n🔗 PUBLIC BROKER...");
  client.setServer(mqtt_server, mqtt_port);
  connectMQTT();
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Verbinde");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500); Serial.print("."); attempts++;
  }
  Serial.println(WiFi.status() == WL_CONNECTED ? 
    "\n✅ WLAN: " + WiFi.localIP().toString() : "\n❌ WLAN FAIL");
}

void connectMQTT() {
  if (client.connect("ESP32_Flowerstore")) {
    Serial.println("✅ ✅ PUBLIC MQTT OK! 🎉");
    // Sofort Test-Publish
    client.publish(mqtt_topic, "ESP32 ONLINE!");
    Serial.println("📤 TEST SENDED!");
  } else {
    Serial.print("❌ rc="); Serial.println(client.state());
  }
}

void loop() {
  if (!client.connected()) {
    Serial.println("🔄 Reconnect...");
    delay(5000);
    return;
  }
  client.loop();
  
  static unsigned long last = 0;
  if (millis() - last > 5000) {
    float temp = 22.5 + random(-20,21)/10.0;
    String payload = "{\"temp\":" + String(temp,1) + ",\"uni\":\"HB\"}";
    client.publish(mqtt_topic, payload.c_str());
    Serial.println("📤 LIVE: " + payload);
    last = millis();
  }
}
