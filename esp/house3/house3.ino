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
#include "mod_http.h"

void setup() {
    Serial.begin(115200);
    initLcd();
    initWiFi();
    initMqtt();
    initParty();
    initRFID();
    initHttp();
}

void loop() {
    loopWiFi();
    loopMqtt();     
    loopParty();
    loopRfid();
    loopLcd();
    loopHttp();
}
