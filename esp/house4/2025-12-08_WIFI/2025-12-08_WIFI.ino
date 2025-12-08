#include <WiFi.h>

const char* ssid = "Lexlan";
const char* password = "gekajo67";

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("📡 Verbinde mit WLAN...");
  WiFi.begin(ssid, password);

  int attempts = 0;

  // Warte auf Verbindung maximal 10 Sekunden (20 * 500 ms)
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Verbunden!");
    Serial.print("IP-Adresse: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n⛔ Verbindung abgebrochen!");
  }
}

void loop() {
  // Keine weiteren Aktionen nötig, Verbindung wird nicht überwacht hier
}
