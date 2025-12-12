#ifndef MOD_WEATHER_LOGIC_H
#define MOD_WEATHER_LOGIC_H
#include <Arduino.h>
#include "mod_weather.h" 

enum WeatherMode {
    WM_NORMAL,
    WM_STORM,
};

extern WeatherMode weatherMode;

// Init & Loop
void initWeatherLogic();
void loopWeatherLogic();

// MQTT-Callback für dieses Haus
// void handleWeatherMqtt(const String& topic, const String& payload);

#endif



// extern Mode currentMode;

// // Haus-spezifische MQTT-Parameter für Haus 1
// extern const char* MQTT_CLIENT_ID_BASE;
// extern const char* TOPIC_BC_STORM;
// extern const char* TOPIC_CMD_STORM;
// extern const char* TOPIC_CMD_NEXT;
// extern const char* TOPIC_STATUS_HOUSE1;
// void handleMqtt(const String& topic, const String& payload);

// // void initParty();
// // void loopParty();

// // Overall
// void startStorm(bool publish);
// void stopStorm(bool publish);
// // void startParty(bool publish);
// // void stopParty(bool publish);
// // void startGas(bool publish);
// // void stopGas(bool publish);

