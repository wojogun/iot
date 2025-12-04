#include <WiFi.h>
#include "config.h"
#include "mod_lcd.h"
#include "mod_wifi.h"

static bool wifiConnected = false;
/*
WiFiClientSecure secureClient;
void initSerial() {
  Serial.begin(115200);
  delay(100);
}
*/
void initWiFi() {
  printLcd("WiFi verbinden", "...", false);
  WiFi.mode(WIFI_STA);
  if (USE_STATIC_IP) {
    Serial.println("Using STATIC IP");
    WiFi.config(local_IP, gateway, subnet, dns);
  } else {
    Serial.println("Using DHCP");
  }

  Serial.print("Verbinden zu: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  uint8_t tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) { // ~20 s Timeout
    delay(500);
    Serial.print(".");
    tries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    IPAddress ip = WiFi.localIP();
    Serial.print("Connected, IP: ");
    Serial.println(ip);
    printLcd("IP:", ip.toString(), false);
  } else {
    wifiConnected = false;
    Serial.println("WiFi FAILED");
    printLcd("WiFi FAIL", "check SSID/PW", true);
  }
}

bool wiFiIsConnected() {
    return (WiFi.status() == WL_CONNECTED);
}

void wiFiLoop() {
  // Auto-Reconnect-Logik
  static unsigned long lastCheck = 0;
  unsigned long now = millis();
  if (now - lastCheck < 5000) return; // alle 5s prüfen
  lastCheck = now;

  if (WiFi.status() != WL_CONNECTED) {
    if (wifiConnected) {
        Serial.println("WiFi lost, reconnecting...");
        wifiConnected = false;
    }
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  } else {
    if (!wifiConnected) {
        wifiConnected = true;
        Serial.print("WiFi reconnected, IP: ");
        Serial.println(WiFi.localIP());
    }
  }
}

