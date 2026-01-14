#include <WiFi.h>
#include "config.h"
#include "mod_config_wifi.h"
#include "mod_sensor_lcd.h"

struct WiFiCredentials {
  const char* ssid;
  const char* pass;
};

static bool wifiConnected = false;
static WiFiCredentials searchWiFi();
static void connectWiFi(const WiFiCredentials& creds);

// WL_CONNECTED ist eine Konstante aus WiFi.h)
bool wiFiIsConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

void initWiFi() {
  printLcd("WiFi verbinden", "...", false);

  WiFi.mode(WIFI_STA);
  if (USE_STATIC_IP) {
    Serial.println("Using STATIC IP");
    // WiFi.config(local_IP, gateway, subnet, dns);
  } else {
    Serial.println("Using DHCP");
  }

  WiFiCredentials creds = searchWiFi();
  connectWiFi(creds);
}

static WiFiCredentials searchWiFi() {
  Serial.println("Scanning WiFi...");
  WiFi.mode(WIFI_STA);

  int n = WiFi.scanNetworks();
  if (n <= 0) {
    Serial.println("Keine Netze gefunden.");
    return { nullptr, nullptr };
  }

  // bekannte Netze in Priorität durchsuchen
  for (uint8_t i = 0; i < WIFI_LIST_COUNT; i++) {  // schleife durch die bekannten netze
    for (int j = 0; j < n; j++) {                  // schleife durch die gefundenen netze   
      if (WiFi.SSID(j) == WIFI_LIST[i].ssid) {
        Serial.print("Gefunden: ");
        Serial.println(WIFI_LIST[i].ssid);
        return { WIFI_LIST[i].ssid, WIFI_LIST[i].pass };
      } else {
        Serial.print("no match: ");
        Serial.println(WIFI_LIST[i].ssid);
      }
    }
  }
  Serial.println("Keines der bekannten WLANs gefunden!");
  return { nullptr, nullptr };
}

static void connectWiFi(const WiFiCredentials& creds) {
  if (creds.ssid == nullptr) {
    Serial.println("Keine bekannte SSID – Verbindung nicht möglich.");
    wifiConnected = false;
    printLcd("WiFi FAIL", "kein Netz", true);
    return;
  }
  
  Serial.print("Verbinde mit: ");
  Serial.println(creds.ssid);
  printLcd("Verbinde mit:", creds.ssid, false);

  WiFi.begin(creds.ssid, creds.pass);

  uint8_t tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) {
    delay(500);
    Serial.print(".");
    printLcd(creds.ssid, String(tries), false);
    tries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    IPAddress ip = WiFi.localIP();
    Serial.print("Connected to ");
    Serial.print(WiFi.SSID());
    Serial.print(", IP: ");
    Serial.println(ip);
    printLcd(WiFi.SSID(), ip.toString(), false);
  } else {
    wifiConnected = false;
    Serial.println("WiFi FAILED");
    printLcd("WiFi FAIL", "check SSID/PW", true);
  }
}

void wiFiLoop() {
  static unsigned long lastCheck = 0;
  static unsigned long lastMasterScan = 0;
  static WiFiCredentials creds;

  unsigned long now = millis();

  if (now - lastCheck >= 5000) {
    lastCheck = now;

    if (WiFi.status() != WL_CONNECTED) {
      if (wifiConnected) {
        Serial.println("WiFi lost, reconnecting...");
        wifiConnected = false;
      }

      WiFi.disconnect(true, true);
      delay(200);

      creds = searchWiFi();
      connectWiFi(creds);
      return;
    } else {
      if (!wifiConnected) {
        wifiConnected = true;
        Serial.print("WiFi reconnected, IP: ");
        Serial.println(WiFi.localIP());
        printLcd(WiFi.SSID(), WiFi.localIP().toString(), false);
      }
    }
  }
  if (WiFi.status() == WL_CONNECTED && strcmp(WiFi.SSID().c_str(), WIFI_LIST[0].ssid) != 0 && (now - lastMasterScan >= 60000)) {
    lastMasterScan = now;
    Serial.println("Nicht im MASTER, prüfe ob MASTER verfügbar ist...");
    creds = searchWiFi();
    if (creds.ssid != nullptr && strcmp(creds.ssid, WIFI_LIST[0].ssid) == 0) {
      Serial.println("MASTER gefunden, wechsle auf MASTER...");
      WiFi.disconnect();
      wifiConnected = false;
      delay(200);
      connectWiFi(creds);
    }
  }
}
