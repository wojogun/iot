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
- **config.cpp** enthält nun die eigentlichen Werte. Da hier auch das WiFi-Passwort und andere Secrets abgespeichert sind, ist diese Datei im Repo nur als **config.cpp.muster** hinterlegt. Die eigentliche config.cpp liegt nur Lokal und sollte auch in der Datei **.gitignore** enthalten sein. ("cpp"=C++ 'plusplus').
Neuerdings werden hier auch alle hausspezifischen MQTT-TOPIC registriezt.

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
extern const WiFiEntry WIFI_LIST[];
extern const uint8_t   WIFI_LIST_COUNT;
```
- für MQTT
```
extern const char* MQTT_SERVER;
extern const int   MQTT_PORT;
extern const char* MQTT_USER;
extern const char* MQTT_PASSWORD;

extern const char* TOPIC_BC_STORM;
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
```
const WiFiEntry WIFI_LIST[] = {
    { "SSID1", "pw1"},
    { "SSID2", "pw2"},
    ...
 }
```
- die IPAddress-Werte braucht man nur, wenn man eine statische IP setzen will
- Mit MDNS_NAME wird der Name Deines Hauses gesetzt. Dieser wird später in der initMDNS() angewendet.
- Unser MQTT-Server ist "5e16dbde757548029c0591f1f71f376c.s1.eu.hivemq.cloud", der Port 8883
- Als MQTT-User, das PW und die Permissions kannst Du unter [hiveMQ](https://console.hivemq.cloud/clusters/5e16dbde757548029c0591f1f71f376c/access-management) (Zugang habe ich euch allen am 2.11.25 gemailt) mit "Add Credentials" setzen. Nutze als Permission "Publish and Subscribe", schließlich wollen wir senden UND empfangen.
- MQTT-Topic zB ```const char* TOPIC_BC_STORM      = "resort/broadcast/storm";```

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
- in der init-Routine werden die Topics subscribed. Das sieht so aus:
```
void initLogic() {
  registerCallbackMqtt(handleMqtt);
  registerRfidCallback(handleRfidSong);
  registerMotionCallback(onMotion);

  subscribeMqtt(TOPIC_BC_STORM);
  subscribeMqtt(TOPIC_BC_GAS);
  //subscribeMqtt(TOPIC_BC_PARTY);  //wird lokal behandelt, daher nur Haus 1,2,4
  //subscribeMqtt(TOPIC_CMD_STORM); // nur Haus 1
  subscribeMqtt(TOPIC_CMD_PARTY);   // nur Haus3
  subscribeMqtt(TOPIC_CMD_SONG);    // nur Haus3
  subscribeMqtt(TOPIC_CMD_NEXT);    // nur Haus3
  Serial.println("Logic subscribed all topics");
  publishSongList();
  mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL");
  mqttClient.publish(TOPIC_STATUSGAS_HOUSE3, "OFF");
  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE3, "OFF");
}
```
## Broadcasts
Es werden verschiedene Broadcasts von den Häusern ausgesendet auf die die anderen reagieren müssen.
- Haus 1 schickt Sturmwarnungen aus und published daher TOPIC_BC_STORM, alle Häuser reagieren bei Sturm - je nach Vorgabe hausspezifisch und gemeinsam (gelbe LED soll blinken, Sturmwarnung wird im LCD angezeigt werden).
- Haus 2 schickt keine Broadcasts aus.
- Haus 3 schickt den Partymodus aus - die Häuser sollen die Fenster schließen.
- Haus 4 schickt eine Gaswarnung aus. Alle blinken rot und Sturmwarnung wird im LCD angezeigt werden.

### Umsetzung der Broadcasts
1) wie bei den Topics schon geschrieben, müssen die BC-Topics subscribed werden.
2) Eventsteuerung
```
void startGas(bool publish) {
  currentGasStatus = ON;
  strip.show();

  printWarnings();
  if (mqttClient.connected()) mqttClient.publish(TOPIC_STATUSGAS_HOUSE3, "ON");
}
void stopGas(bool publish) {
  currentGasStatus = OFF;
  strip.show();
  switchLed(false);
  printWarnings();
  if (mqttClient.connected()) mqttClient.publish(TOPIC_STATUSGAS_HOUSE3, "OFF");
}

void startStorm(bool publish) {
  currentStormStatus = ON;
  ctrWindow(WINDOW_CLOSED);
  ctrDoor(DOOR_CLOSED);  strip.clear();
  strip.show();
  buzzer.playTone(0, 0);
  printWarnings();
  if (publish && mqttClient.connected())  mqttClient.publish(TOPIC_BC_STORM, "ON");
  if            (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE3, "ON");
}
void stopStorm(bool publish) {
  currentStormStatus = OFF;
  ctrWindow(WINDOW_OPEN);
  ctrDoor(DOOR_OPEN);
  switchLed(false);
  printWarnings();
  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_STORM, "OFF");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL");
  }
  if (mqttClient.connected())  mqttClient.publish(TOPIC_STATUSSTORM_HOUSE3, "OFF");
}

void printWarnings() {
  uint8_t warnMask = (currentStormStatus == ON ? 0b10 : 0) | (currentGasStatus == ON ? 0b01 : 0);
  switch (warnMask) {
    case 0b00:
      printLcd("Next:", (nextPartyText == "" ? "keine Buchung" : nextPartyText ), false);
      switchLed(false);
      rgbOff();
      break;
    case 0b10:
      printLcd("STURMWARNUNG", "", false);
      blinkLed();
      rgbOff();
      break;
    case 0b01:
      printLcd("GASWARNUNG", "", false);
      switchLed(false);
      rgbBlink(255,0,0);
      break;
    case 0b11:
      printLcd("STURM + GAS", "WARNUNG", false);
      rgbBlink(255,0,0);
      blinkLed();
      break;
  }
}
```
--> Die Funktionen für die gelbe LED und die RGB-LED sind in hardware.h/.cpp, für das Display in mod_lcd.h/cpp

## Zusammenspiel der Funktionen
Das Modul mqtt ist seit dem letzten refactoring-Durchlauf komplett generisch. Alle Definitionen sind nun in der Hauslogik.
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

```
# Empfehlung für dem "Hausbau"
1. neuen Sketch anlegen
**Name:** HausX.ino
**Inhalt:** erst mal das Minimum
```
#include <Arduino.h>
#include "config.h"
#include "hardware.h"     // Pinbelegung, LED, RGB, Servos
#include "mod_wifi.h"     // WLAN
#include "mod_mqtt.h"
#include "mod_lcd.h"      // Display am Haus
#include "mod_logic.h"    // alles was dein Haus an Logik braucht

void setup() {
    Serial.begin(115200);
    initLcd();
    initWiFi();
    initMqtt();
    initLogic();
    // und was sonst noch gebraucht wird
}

void loop() {
    loopLcd();
    loopWiFi();
    loopMqtt();
    loopLogic();
    loopYellowLed();
    loopRgb();
}
```
2. Basis Module hineinkopieren (jeweils .h und .cpp):
    - config
    - hardware
    - mod_lcd
    - mod_mqtt
    - mod_wifi
3. Zusätzliche Hardwaremodule (nach Bedarf - nicht vergessen die init und loop-Routinen im Hauptfile zu ergänzen!)
    - mod_button
    - mod_rfid
    - mod_http (für eine lokale Steuerungsseite)
    - oder so ein Modul selbst schreiben. Aber nicht alle Aktoren des Hauses brauchen ein eigenes Modul
      - **Fenster** ```ctrWindow(WINDOW_OPEN / WINDOW_CLOSED);```
      ACHTUNG: der Servo tendiert zu heiß werden. Bei wird wird auch das Display deutlich schwerer lesbar, wenn das Fenster initialisiert wird. Deshalb habe ich bei mir das Fenster ausgeschaltet. Allerdings habe ich den Verdacht, dass der Servo kaputt ist. Suche in hardware.cpp nach ```const bool USE_WINDOW   = false;``` und ändere auf true!
      - **Tür** ```ctrDoor(DOOR_OPEN / DOOR_CLOSED);```
      Die Temperatur des Door-Servos lässt sich nicht so leicht überprüfen, da er tief drinnen verbaut ist. Dieser Motor hat bei mir allerdings keine Probleme gemacht, daher ist er aktiviert: ```const bool USE_DOOR     = true;```
      - das **RGB-Modul** ist als "strip" schon in der Hardware definiert. 
      - die **gelbe LED** lässt sich mit ```digitalWrite(PIN_LED_YELLOW, bedingung ? HIGH : LOW);``` ein und ausschalten
      - **Ventilator** ```ctrFan(FAN_ON bzw. FAN_OFF);```
      - **Hum & Temp** tba
      - **Bewegungsmelder** PIN 14 ist bereits in hardware definiert
4. Erstellen eines Logik-Moduls für das Haus
In diesem werden dann alle Events abgehandelt
5. Anpassen der config.cpp

# Hardwaremodule

## Buttons
mod_button.h/cpp

in der Logik-Datei:
```
#include "mod_button.h"

void initLogic() {
  initHardware();
  initButton(btnPartyStart);
  initButton(btnPartyStop);
}

void loopLogic() {
  unsigned long now = millis();
  ButtonEvent ev1 = updateButton(btn1, now);
    if (ev1 == BUTTON_LONG) startParty(true);
  ButtonEvent ev2 = updateButton(btn2, now);
    if (ev2 == BUTTON_LONG) stopParty(true);
}

Als Button-Event können
- BUTTON_NONE
- BUTTON_SHORT
- BUTTON_LONG
- BUTTON_DOUBLE
verwendet werden
