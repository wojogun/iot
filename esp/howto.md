Ein paar kleine Learning aus meinem bisherigen Häuslbaun:
# Muster-Sketches
sind ausgesprochen hilfreich, um die Funktionen des Hauses zu verstehen. Auch die Pin-Belegungen kann man ihnen entnehmen - z.B. ist der Buzzer über Pin 25 angeschlossen.

# Config auslagern
In der C/C++ Welt ist es üblich die Deklaration und Definition zu trennen. Das soll Wiederverwendbarkeit, klare Schnittstellen und weniger Chaos im Hauptsketch bringen.
- **config.h** enthält nur Deklarationen und Konstanten, aber keine Werte. 
- **config.cpp** enthält nun die eigentlichen Werte. Da hier auch das WiFi-Passwort und andere Secrets abgespeichert sind, ist diese Datei im Repo nur als **config.cpp.muster** hinterlegt. Die eigentliche config.cpp liegt nur Lokal und sollte auch in der Datei **.gitignore** enthalten sein.

## config.h
Was passiert hier drinnen?
### Header Guards
```
#ifndef CONFIG_H
#define CONFIG_H
```
Das sind so genannte Header Guards, die bewirken, dass die config nur ein mal ausgeführt wird - "if not defined". 

### Grundfunktionen
```
#include <Arduino.h>
```
Diese Datei macht das Projekt arduino-tauglich indem grundlegende Funktionen bereitstellt werden.

### Klasse für statische IP
```
#include <IPAddress.h>
```
Damit kann man die Klasse IPAddress verwenden, die in der config.h verwendet wird. Will man nur DHCP verwenden, kann man das auch weglassen - man muss allerdings auch alle Referenzen entfernen.
```
extern IPAddress local_IP;
extern IPAddress gateway;
extern IPAddress subnet;
extern IPAddress dns;
```

### Variable
- für das WLAN
```
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
```
- für MQTT
```
extern const char* MQTT_SERVER;
extern const int   MQTT_PORT;
extern const char* MQTT_USER;
extern const char* MQTT_PASSWORD;
```

- für mDNS Name 
```
extern const char* MDNS_NAME;  
```
mDNS = Multicast DNS (auch bekannt als Bonjour u.a.) ist ein Netzwerkprotokoll, das es Geräten erlaubt, sich ohne DNS-Server gegenseitig über Namen zu finden.  

- ein Zertifikat
```
extern const char ROOT_CA[] PROGMEM;
```
allerdings können wir das mit dem free HiveMQ-Abo nicht verwenden, das geht erst in den Bezahl-Versionen.


## config.cpp
Hier werden die zuvor in der config.h deklarierten Variablen gesetzt. 
- Bei WIFI_SSID und WIFI_PASSWORD trage die Anmeldedaten Deines Hotspots ein!
- die IPAddress-Werte braucht man nur, wenn man eine statische IP setzen will
- Mit MDNS_NAME wird der Name Deines Hauses gesetzt. Dieser wird später in der initMDNS() angewendet.
- Unser MQTT-Server ist "5e16dbde757548029c0591f1f71f376c.s1.eu.hivemq.cloud", der Port 8883
- Als MQTT-User, das PW und die Permissions kannst Du unter https://console.hivemq.cloud/clusters/5e16dbde757548029c0591f1f71f376c/access-management mit "Add Credentials" setzen. Nutze als Permission "Publish and Subscribe", schließlich wollen wir senden UND empfangen.


# MQTT
## Topics
Mein Haus verwendet drei Kategorien von MQTT-Topics:
- Haus-spezifische Kommandos, also Kommandos, die nur Haus 3 betreffen
- Resort-weite Broadcasts, also Nachrichten, die alle Häuser erhalten
- Statusmeldungen: Haus 3 sendet Informationen Richtung Node-RED oder andere Häuser

Die Topics sind so aufgebaut: "resort/[wenBetriffEs]/[wasBetrifftEs]/[ev. ein Kommando]":
- TOPIC_CMD_STORM = "resort/house3/storm/cmd"  --> verlangt eine Payload, in diesem Fall werden "ON" und "OFF" verarbeitet.
```
else if (t == TOPIC_CMD_STORM) {
    if (msg == "ON") startStorm(false);
    else if (msg == "OFF") stopStorm(false);
}
```
Das "false" verhindert neuerliches Broadcasten, wenn das Haus reagiert. Die Funktion startStorm erwartet den Parameter "publish". Nur wenn man wirklich etwas veröffentlichen will, setzt man den Wert auf "true" Damit werden Schleifen vermieden. Storm sollte von Haus 1 initiiert und bei allen Häusern den Sturm-Modus auslösen.

## Zusammenspiel der Funktionen
- maintainMQTT() muss ständig laufen, damit Nachrichten empfangen werden.
- reconnectMQTT() verbindet den ESP32 wieder zum MQTT-Broker. Hier werden Abonnements gesetzt.
- mqttCallback(topic, payload, length) wird immer aufgerufen, wenn eine MQTT-Nachricht reinkommt und reagiert je nach Topic.
- startStorm(bool publish) bzw stopStorm(bool publish)

## Funktionsmapping (Was löst welches Topic aus?)
```
 TOPIC_CMD_PARTY (nur Haus 3)
     ├── START → startParty(false)
     └── STOP  → stopParty(false)

 TOPIC_CMD_STORM (Nur Haus 1)
     ├── ON  → startStorm(false)
     └── OFF → stopStorm(false)

 TOPIC_BC_PARTY (broadcast an alle Häuser)
     ├── ON  → startStorm(false)
     └── OFF → stopStorm(false)

 TOPIC_BC_STORM (broadcast an alle Häuser)
     ├── ON  → startStorm(false)
     └── OFF → stopStorm(false)

 TOPIC_STATE_STORM   (optional)
     ├── ON  → resortStormActive = true;  startStorm(false)
     └── OFF → resortStormActive = false; stopStorm(false)

 TOPIC_STATE_PARTY   (optional)
     ├── ON  → resortPartyActive = true
     └── OFF → resortPartyActive = false

```

Die Topics sollten dann in der Hauslogik verarbeitet werden (siehe Muster mod_partylogic.cpp). Beispiel Sturm-Modus, den jedes Haus haben sollte:
```
void startStorm(bool publish) {
  currentMode = MODE_STORM;
  ctrWindow(WINDOW_CLOSED);
  ctrDoor(DOOR_CLOSED);
  strip.clear();
  strip.show();
  buzzer.playTone(0, 0);

  digitalWrite(PIN_LED_YELLOW, HIGH);
  printLcd(	"STURMWARNUNG", "", true);	// schreibt "STURMWARNUNG" in die 1. Zeile und "" in die 2. Zeile. true=Text blinkt

  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_STORM, "ON");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "STORM");
  }
}
```
startStorm(false) wird widerum aus der mod_mqtt.cpp aufgerufen, wo eine Callbackfunktion auf die Payload-Änderung des Topics wartet. Der Parameter false bezieht sich auf, ob die Nachricht wieder gepublished werden soll - in diesem Fall keineswegs, da sonst eine Schleife zustandekommt.
```
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String t = String(topic);
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  if ...
  else if (t == TOPIC_CMD_STORM || t == TOPIC_BC_STORM) {
    if (msg == "ON") {
      startStorm(false);
    } else if (msg == "OFF") {
      stopStorm(false);
    }
```
