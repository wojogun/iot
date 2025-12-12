#include "mod_temperatur_huminity.h"
#include "hardware.h"
#include <xht11.h>

static xht11 sensor_xht(PIN_TEMPHUM);
static unsigned char rawData[4] = {0, 0, 0, 0};

void initTempHum() {
    pinMode(PIN_TEMPHUM, INPUT);
    Serial.println("XHT11 init done");
}

TempHumData readTempHum() {
    TempHumData data;

    if (sensor_xht.receive(rawData)) {
        // wie im Demo:
        // buf[0] = Humidity (int),
        // buf[2] = Temperature (int)
        data.hum  = rawData[0];
        data.temp = rawData[2];
    } else {
        data.hum  = -1;
        data.temp = -100;   // „offensichtlich falsch“
    }

    return data;
}
