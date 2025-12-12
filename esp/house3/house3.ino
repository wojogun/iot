/************************************************************
 * Haus 3 – Partyhaus (Wolfi)
 ************************************************************/
#include <Arduino.h>
#include "config.h"
#include "hardware.h"
#include "mod_wifi.h"
#include "mod_mqtt.h"
#include "mod_lcd.h"
#include "mod_rfid.h"
#include "mod_partylogic.h"
#include "mod_http.h"
#include "mod_button.h"
#include "mod_motion.h"

void setup() {
    Serial.begin(115200);
    initHardware();
    initButton(btn1);
    initButton(btn2);

    initLcd();
    initWiFi();
    initMqtt();
    initRFID();
    initMotion();
    initParty();
    initHttp();
}

void loop() {
    loopWiFi();
    loopMqtt();     
    loopParty();
    loopRfid();
    loopLcd();
    loopMotion();
    loopHttp();
}
