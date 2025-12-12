/************************************************************
 * Haus 1 – Weather & Storm (Lisa)
 ************************************************************/
#include <Arduino.h>
#include "config.h"
#include "hardware.h"
#include "mod_wifi.h"
#include "mod_mqtt.h"
#include "mod_lcd.h"
#include "mod_weather.h"
#include "mod_weather_logic.h"
#include "mod_motion.h"
#include "mod_led.h"
#include "mod_storm_logic.h"
// ggf. Webserver / HTML

void setup() {
    Serial.begin(115200);
    initHardware();
    initLcd();
    initWiFi();
    initMqtt();

    //Haus 1 Weather Logic
    initWeather();      // Temp/Hum/Steam/Wind
    initWeatherLogic();
    initHouseLed();     // LED-Modul
    initMotion();       // PIR
    initStormLogic();

    // initRFID();  
    // initPartyLogic();            // Haus 3 Party Logic)
                   
    // init Webserver, Zeit, etc.
}

void loop() {
    wiFiLoop();
    mqttLoop();             // oder maintainMQTT(), siehe oben
    lcdUpdate();            // Display aktualisieren
    loopWeatherLogic();     // Wetter + Sturm + MQTT --> NEU Storm Logic!
    loopMotion();           // TODO: Bewegungslogik einbauen
    loopStormLogic();       // Wetter messen, Sturm entscheiden, MQTT senden
    // rfidLoop();
    // partyLoop();
    
    // HTTP-Server bearbeiten, etc.
}
