#include "mod_weather_logic.h"
#include "mod_lcd.h"
#include "mod_mqtt.h"
#include "mod_steam.h"
#include "mod_wind.h"
#include "config.h"
#include "hardware.h"

WeatherMode weatherMode = WM_NORMAL;

// ---- Schwellwerte für „Sturm“ (einfaches Modell) ----
// Werte musst du ggf. mit echten Messungen fein-tunen!
static const float TEMP_HOT_THRESHOLD   = 28.0;   // sehr warm
static const float TEMP_COLD_THRESHOLD  =  0.0;   // sehr kalt
// static const int   STEAM_WET_THRESHOLD  = 2500;   // fällt weg, kommt von mod_steam.cpp !!!
static const int   WIND_STRONG_THRESHOLD = 700;   // Platzhalter, falls du später Wind simulierst

void initWeatherLogic() {
    weatherMode = WM_NORMAL;
    printLcd("House 1 Ready", "Weather & Storm", false);
    Serial.println("initWeatherLogic: done");
}

void loopWeatherLogic() {
    static unsigned long last = 0;

    // alle 5 Sekunden messen & senden
    if (millis() - last < 5000) return;
    last = millis();

    // --- 1. Sensoren auslesen ---
    WeatherData d = readWeather();

    // --- 2. LCD-Ausgabe ---
    String line1 = String(d.temp, 1) + "C  " + String(d.hum, 0) + "%";
    String line2 = "Steam:" + String(d.steam);
    printLcd(line1, line2, false);

    // --- 3. Sturm-Erkennung (sehr einfache Logik) ---
    bool storm = false;

    // Variante 1: Nur viel „Steam“ = Regen / Nebel
    if (d.steam > STEAM_WET_THRESHOLD) {
        storm = true;
    }

    // Optional: Kombi-Fälle (kannst du später fein justieren)
    // if (d.steam > STEAM_WET_THRESHOLD && d.wind > WIND_STRONG_THRESHOLD) { ... }

    // --- 4. Reaktion auf Sturm: LED + Fenster ---
    if (storm) {
        if (weatherMode != WM_STORM) {
            Serial.println("Storm detected -> LED on, window closed");
        }
        weatherMode = WM_STORM;
        digitalWrite(PIN_LED_YELLOW, HIGH);   // gelbe Haus-LED an
        ctrWindow(WINDOW_CLOSED);             // Fenster sicher zu
        // ggf. später: Buzzer, Fan, Tür, etc.
    } else {
        if (weatherMode != WM_NORMAL) {
            Serial.println("Storm ended -> LED off");
        }
        weatherMode = WM_NORMAL;
        digitalWrite(PIN_LED_YELLOW, LOW);    // LED aus
    }

    // --- 5. JSON mit allen Werten + Sturm-Flag senden ---
    String json = "{\"temp\":";
    json += d.temp;
    json += ",\"hum\":";
    json += d.hum;
    json += ",\"steam\":";
    json += d.steam;
    json += ",\"wind\":";
    json += d.wind;
    json += ",\"storm\":";
    json += (storm ? 1 : 0);
    json += "}";

    publishMqtt(TOPIC_WEATHER_VALUES, json);

    // --- 6. Einzel-Topics senden ---
    publishMqtt(TOPIC_TEMP, String(d.temp));
    publishMqtt(TOPIC_HUM, String(d.hum));
    publishMqtt(TOPIC_STEAM, String(d.steam));
    publishMqtt(TOPIC_WIND, String(d.wind));
    publishMqtt(TOPIC_BC_STORM, storm ? "1" : "0");
  }



