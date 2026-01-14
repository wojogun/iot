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
    WindData wind       = readWind();
    // SteamData steam     = readSteam();


    weatherValues.temp  = tempHum.temp;
    weatherValues.hum   = tempHum.hum;
    weatherValues.wind  = wind.level;
    // weatherValues.steam = steam.raw;     // Für Analoge Wert (0-4095) 
    weatherValues.steam = getSteamPercent();


    return weatherValues;
}
