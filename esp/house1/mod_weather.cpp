#include "mod_weather.h"
#include "mod_sensor_temperatur_huminity.h"
#include "mod_sensor_steam.h"
#include "mod_sensor_wind.h"

// --- Funktiuoniert als Aggregator - Fügt alle Wetterdaten zusammen ---

void initWeather() {
    initTempHum();
    initSteam();
    initWind();
}

WeatherData readWeather() {
    WeatherData weatherValues{};

    TempHumData tempHum = readTempHum();
    SteamData steam     = readSteam();
    WindData wind       = readWind();

    weatherValues.temp  = tempHum.temp;
    weatherValues.hum   = tempHum.hum;
    weatherValues.steam = steam.raw;
    weatherValues.wind  = wind.level;

    return weatherValues;
}
