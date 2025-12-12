

#ifndef MOD_WEATHER_H
#define MOD_WEATHER_H

#include <Arduino.h>

struct WeatherData {
    float temp;
    float hum;
    int steam;
    int wind;
};

void initWeather();
WeatherData readWeather();

#endif
