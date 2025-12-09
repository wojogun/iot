Ein paar kleine Learning aus meinem bisherigen Häuslbaun:

# Tutorial beachten
[Tutorial für KS5009](https://docs.keyestudio.com/projects/KS5009/en/latest/docs/Arduino/arduino.html#getting-started-with-arduino)

# Muster-Sketches
sind ausgesprochen hilfreich, um die Funktionen des Hauses zu verstehen. Auch die Pin-Belegungen kann man ihnen entnehmen - z.B. ist der Buzzer über Pin 25 angeschlossen.
Einfach Punkt für Punkt durchgehen. Am Ende hast Du Arduino IDE, cp2012 und Demosketches.

# Eigenen Sketch anlegen
ich hab meinen haus3_party genannt. Die Erweiterung dieses Files wäre ".ino". Natürlich kann man nun alles in einem File lassen aber dann wird es sehr schnell unübersichtlich. Daher habe ich die config, die Hardwaredefinitionen und sonstiges in eigene Module (mod_...) ausgelagert.

# Config auslagern
In der C/C++ Welt ist es üblich die Deklaration und Definition zu trennen. Das soll Wiederverwendbarkeit, klare Schnittstellen und weniger Chaos im Hauptsketch bringen.
- **config.h** enthält nur Deklarationen und Konstanten, aber keine Werte. ("h"=header")
- **config.cpp** enthält nun die eigentlichen Werte. Da hier auch das WiFi-Passwort und andere Secrets abgespeichert sind, ist diese Datei im Repo nur als **config.cpp.muster** hinterlegt. Die eigentliche config.cpp liegt nur Lokal und sollte auch in der Datei **.gitignore** enthalten sein. ("cpp"=C++ 'plusplus')

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
const WiFiEntry WIFI_LIST[] = {
    { "SSID1", "pw1"},
    { "SSID2", "pw2"},
    ...
 }
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
- ~~Bei WIFI_SSID und WIFI_PASSWORD trage die Anmeldedaten Deines Hotspots ein!~~
die WiFi-SSID und Pws werden nun als Array hinterlegt. Das erste Paar hat Prio 1, die anderen sind Fallback. Damit kannst Du einen Hotspot schon für die Demo als Prio1 definieren, deinen Handyhotspot als Prio2 und dein WLAN zu Hause als Prio3 - und hast in jedem Fall irgendein Netz!
- die IPAddress-Werte braucht man nur, wenn man eine statische IP setzen will
- Mit MDNS_NAME wird der Name Deines Hauses gesetzt. Dieser wird später in der initMDNS() angewendet.
- Unser MQTT-Server ist "5e16dbde757548029c0591f1f71f376c.s1.eu.hivemq.cloud", der Port 8883
- Als MQTT-User, das PW und die Permissions kannst Du unter [hiveMQ](https://console.hivemq.cloud/clusters/5e16dbde757548029c0591f1f71f376c/access-management) (Zugang habe ich euch allen am 2.11.25 gemailt) mit "Add Credentials" setzen. Nutze als Permission "Publish and Subscribe", schließlich wollen wir senden UND empfangen.

# MQTT
## Topics
Mein Haus verwendet drei Kategorien von MQTT-Topics, die alle in config.cpp/.h definiert werden:
- Haus-spezifische Kommandos, also Kommandos, die nur Haus 3 betreffen
- Resort-weite Broadcasts, also Nachrichten, die alle Häuser erhalten
- Statusmeldungen: Haus 3 sendet Informationen Richtung Node-RED oder andere Häuser
Die Abarbeitung findet im haus-Logik-Modul statt. Bei mir also in mod_partylogic.cpp

Der Topic-Name ist "TOPIC" + was das ist (BC=Broadcast, CMD=Kommando) + was es betrifft
Die Topics sind so aufgebaut: "resort/[wenBetriffEs]/[wasBetrifftEs]/[ev. ein Kommando]":
- TOPIC_CMD_STORM = "resort/house3/storm/cmd"  --> verlangt eine Payload, in diesem Fall werden "ON" und "OFF" verarbeitet.
```
else if (t == TOPIC_CMD_STORM) {
    if (msg == "ON") startStorm(false);
    else if (msg == "OFF") stopStorm(false);
}
```
Das "false" verhindert neuerliches Broadcasten, wenn das Haus reagiert. Die Funktion startStorm erwartet den Parameter "publish". Nur wenn man wirklich etwas veröffentlichen will, setzt man den Wert auf "true" Damit werden Schleifen vermieden. Storm sollte von Haus 1 initiiert und bei allen Häusern den Sturm-Modus auslösen. Diese verlangt nun
- handelMqtt(topic, payload) Hier wird mqtt-in behandelt:
```
  if (topic == TOPIC_CMD_PARTY) {
    if (payload == "START") startParty(false);
    else if (payload == "STOP") stopParty(false);
  }
  ```
- in der init-Routine werden die Topics subscribed. Bei mir sieht das so aus:
```
void initParty() {
  initHardware();
  registerCallbackMqtt(handleMqtt);

  subscribeMqtt(TOPIC_BC_STORM);
  //subscribeMqtt(TOPIC_BC_PARTY); wird lokal behandelt
  subscribeMqtt(TOPIC_CMD_PARTY);
  subscribeMqtt(TOPIC_CMD_STORM);
  subscribeMqtt(TOPIC_CMD_SONG);
  subscribeMqtt(TOPIC_CMD_NEXT);
  // TOPIC_STATUS_HOUSE3  = "resort/house3/status";      --> publish only
  // TOPIC_CURRENT_SONG   = "resort/house3/party/song";  --> publish only

  Serial.println("Partylogic subscribed all topics");
}```

Das Modul mqtt ist seit dem letzten refactoring-Durchlauf komplett generisch. Alle Definitionen sind nun in der Hauslogik
## Zusammenspiel der Funktionen
- maintainMQTT() muss ständig laufen, damit Nachrichten empfangen werden.
- reconnectMQTT() verbindet den ESP32 wieder zum MQTT-Broker. Hier werden Abonnements gesetzt.
- mqttCallback(topic, payload, length) wird immer aufgerufen, wenn eine MQTT-Nachricht reinkommt und reagiert je nach Topic.
- startStorm(bool publish) bzw stopStorm(bool publish)

## Funktionsmapping (Was löst welches Topic aus?)
```
 TOPIC_CMD_PARTY
     ├── START → startParty(false)
     └── STOP  → stopParty(false)

 TOPIC_CMD_STORM
     ├── ON  → startStorm(false)
     └── OFF → stopStorm(false)

 TOPIC_BC_STORM
     ├── ON  → startStorm(false)
     └── OFF → stopStorm(false)

 TOPIC_STATE_STORM   (optional)
     ├── ON  → resortStormActive = true;  startStorm(false)
     └── OFF → resortStormActive = false; stopStorm(false)

 TOPIC_STATE_PARTY   (optional)
     ├── ON  → resortPartyActive = true
     └── OFF → resortPartyActive = false

```
