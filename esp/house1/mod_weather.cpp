#include "mod_weather.h"
#include "mod_temperatur_huminity.h"
#include "mod_steam.h"
#include "mod_wind.h"

// --- Funktiuoniert als Aggregator - Fügt alle Wetterdaten zusammen ---

void initWeather() {
    initTempHum();
    initSteam();
    initWind();
}

WeatherData readWeather() {
    WeatherData wData;

    TempHumData tempHum = readTempHum();
    SteamData   steam = readSteam();
    WindData    wind = readWind();

    wData.temp  = tempHum.temp;
    wData.hum   = tempHum.hum;
    wData.steam = steam.rawData;
    wData.wind  = wind.level;

    return wData;
}
