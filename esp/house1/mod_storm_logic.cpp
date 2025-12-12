#include "mod_storm_logic.h"
#include "mod_weather.h"
#include "mod_motion.h"
#include "mod_led.h"
#include "hardware.h"
#include "config.h"
#include "mod_mqtt.h"
#include "mod_lcd.h"
#include "mod_steam.h"
#include "mod_wind.h"

// Dieses Modul ersetzt den "mod_weater_logic" (war basic/beginn)

// --- liest WeatherData über mod_weather
// --- holt Bewegung über mod_motion
// --- steuert LED & Fenster
// --- publiziert MQTT (Sensor-Werte + Sturm)
// --- reagiert auf Node-RED-Kommandos (AUTO/ON/OFF, Wind)



StormMode  stormMode  = STORMMODE_AUTO;
StormState stormState = STORMSTATE_NORMAL;

// einfache Schwellwerte – TODO später anpassen!!
static const int   WIND_STORM_THRESHOLD  = 70;    // 0..100
// STEAM_WET_THRESHOLD kommt aus mod_steam.cpp !!! 

// TODO optional: Motion-Callback – z.B. für Perimeterschutz
static void onMotionChanged(bool active) {
    if (active && stormState == STORMSTATE_NORMAL) {
        // Kurzes Feedback: „Bewegung erkannt“
        blinkHouseLed(2, 100, 100);
    }
}

void initStormLogic() {
    Serial.println("Init Storm Logic (House 1)");

    stormMode  = STORMMODE_AUTO;
    stormState = STORMSTATE_NORMAL;

    // LED & Fenster in initial Zustand 
    initHouseLed();
    setHouseLed(false);
    ctrWindow(WINDOW_OPEN);   // Standard open

    // Motion-Callback registrieren
    registerMotionCallback(onMotionChanged);

    // MQTT-Callback & Subscriptions
    registerCallbackMqtt(handleStormMqtt);
    subscribeMqtt(TOPIC_STORM_CMD);
    subscribeMqtt(TOPIC_WIND_CMD);

    // optional: Status senden
    publishMqtt(TOPIC_STATUS_HOUSE1, "NORMAL");
}

void handleStormMqtt(const String& topic, const String& payload) {
    Serial.print("StormMQTT in [");
    Serial.print(topic);
    Serial.print("]: ");
    Serial.println(payload);

    if (topic == TOPIC_STORM_CMD) {
        // payload: "AUTO", "ON", "OFF"
        if (payload == "AUTO") {
            stormMode = STORMMODE_AUTO;
        } else if (payload == "ON") {
            stormMode = STORMMODE_FORCE_ON;
        } else if (payload == "OFF") {
            stormMode = STORMMODE_FORCE_OFF;
        }
        Serial.print("StormMode set to: ");
        Serial.println(payload);
    }
    else if (topic == TOPIC_WIND_CMD) {
        // Slider in Node-RED → Wind simulieren
        int lvl = payload.toInt();
        setSimulatedWind(lvl);
        Serial.print("Simulated wind set to: ");
        Serial.println(lvl);
    }
}

void loopStormLogic() {
    static unsigned long last = 0;
    if (millis() - last < 5000) return;   // alle 5 Sekunden
    last = millis();

    // --- 1. Wetterdaten holen (via Aggregator) ---
    WeatherData wd = readWeather();   // nutzt intern Temp/Hum, Steam, Wind

    // --- 2. LCD-Anzeige ---
    String line1 = String(wd.temp, 1) + "C  " + String(wd.hum, 0) + "%";
    String line2 = "Steam:" + String(wd.steam);
    printLcd(line1, line2, false);

    // --- 3. Automatische Sturm-Erkennung ---
    bool autoStorm = false;

    // Regen/Nebel reicht schon für „Sturmwarnung“
    if (wd.steam > STEAM_WET_THRESHOLD) {
        autoStorm = true;
    }

    // Starker Wind: extra Bedingung
    if (wd.wind >= WIND_STORM_THRESHOLD) {
        autoStorm = true;
    }

    // hier könntest du später noch Temperaturlogik einbauen
    // (z.B. Schneesturm, Hitze etc.)

    // --- 4. Modus berücksichtigen (AUTO vs FORCE) ---
    bool finalStorm = false;
    switch (stormMode) {
        case STORMMODE_FORCE_ON:
            finalStorm = true;
            break;
        case STORMMODE_FORCE_OFF:
            finalStorm = false;
            break;
        case STORMMODE_AUTO:
        default:
            finalStorm = autoStorm;
            break;
    }

    StormState newState = finalStorm ? STORMSTATE_STORM : STORMSTATE_NORMAL;

    // --- 5. Zustand ändern & Aktionen ausführen ---
    if (newState != stormState) {
        stormState = newState;
        if (stormState == STORMSTATE_STORM) {
            Serial.println(">>> STORM ACTIVE");

            setHouseLed(true);
            ctrWindow(WINDOW_CLOSED);

            publishMqtt(TOPIC_STATUS_HOUSE1, "STORM");
            publishMqtt(TOPIC_STORM_ALERT, "1");

            // Broadcast an alle Häuser, damit sie Fenster/Türen schließen
            publishMqtt(TOPIC_BC_STORM, "ON");
        } else {
            Serial.println(">>> STORM ENDED");

            setHouseLed(false);
            ctrWindow(WINDOW_OPEN);

            publishMqtt(TOPIC_STATUS_HOUSE1, "NORMAL");
            publishMqtt(TOPIC_STORM_ALERT, "0");

            publishMqtt(TOPIC_BC_STORM, "OFF");
        }
    }

    // --- 6. Werte nach außen geben (JSON + Einzelwerte) ---
    String json = "{\"temp\":";
    json += wd.temp;
    json += ",\"hum\":";
    json += wd.hum;
    json += ",\"steam\":";
    json += wd.steam;
    json += ",\"wind\":";
    json += wd.wind;
    json += ",\"storm\":";
    json += (stormState == STORMSTATE_STORM ? 1 : 0);
    json += "}";

    publishMqtt(TOPIC_WEATHER_VALUES, json);
    publishMqtt(TOPIC_TEMP,        String(wd.temp));
    publishMqtt(TOPIC_HUM,         String(wd.hum));
    publishMqtt(TOPIC_STEAM, String(wd.steam));
    publishMqtt(TOPIC_WIND,  String(wd.wind));
}
