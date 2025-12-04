/************************************************************
 * Haus 3 – Partyhaus (Wolfi)
 ************************************************************/
#include <Arduino.h>
#include "config.h"
#include "mod_wifi.h"
#include "mod_mqtt.h"
#include "mod_lcd.h"
#include "mod_rfid.h"
#include "mod_partylogic.h"
// ggf. Webserver / HTML

void setup() {
    Serial.begin(115200);
    initLcd();
    initWiFi();
    initMQTT();
    initPartyLogic();
    initRFID();
    // init Webserver, Zeit, etc.
}

void loop() {
    wiFiLoop();
    mqttLoop();          // oder maintainMQTT(), siehe oben
    partyLoop();
    rfidLoop();
    lcdUpdate();
    // HTTP-Server bearbeiten, etc.
}
