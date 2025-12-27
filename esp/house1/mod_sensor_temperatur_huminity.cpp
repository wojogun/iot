#include "hardware.h"
#include "mod_sensor_temperatur_huminity.h"
#include <xht11.h>

static xht11 sensor_xht(PIN_TEMP_HUM);
static unsigned char raw[4] = {0, 0, 0, 0};

static bool simEnabled  = false;
static float simTemp    = 20.0f;
static float simHum     = 50.0f;

// internal cache to avoid sensor spam
static TempHumData lastGood{};
static bool hasLastGood = false;
static unsigned long lastReadMs = 0;
static const unsigned long SENSOR_MIN_INTERVAL_MS = 1200; // IMPORTANT

void setTempHumSimulationEnabled(bool on) { simEnabled = on; }
void setSimulatedTemp(float t) { simTemp = t; }
void setSimulatedHum(float h)  { simHum = h; }

void initTempHum() {
  pinMode(PIN_TEMP_HUM, INPUT);
  Serial.println("XHT11 init done");
}

TempHumData readTempHum() {
  TempHumData data{};

  if (simEnabled) {
    data.temp = simTemp;
    data.hum  = simHum;
    lastGood = data;
    hasLastGood = true;
    return data;
  }

  unsigned long now = millis();
  if (hasLastGood && (now - lastReadMs < SENSOR_MIN_INTERVAL_MS)) {
    return lastGood;  // return cached value
  }
  lastReadMs = now;

  if (sensor_xht.receive(raw)) {
    data.hum  = raw[0];
    data.temp = raw[2];
    lastGood = data;
    hasLastGood = true;
    return data;
  }

  // if failed, do NOT jump to -100/-1, keep last good if possible
  if (hasLastGood) return lastGood;

  // first read failed -> return error
  data.hum  = -1;
  data.temp = -100;
  return data;
}


// #include "hardware.h"
// #include "mod_sensor_temperatur_huminity.h"
// #include <xht11.h>

// static xht11 sensor_xht(PIN_TEMP_HUM);
// static unsigned char raw[4] = {0, 0, 0, 0};

// static bool simEnabled  = false;
// static float simTemp    = 20.0f;
// static float simHum     = 50.0f;

// void setTempHumSimulationEnabled(bool on) { simEnabled = on; }
// void setSimulatedTemp(float t) { simTemp = t; }
// void setSimulatedHum(float h)  { simHum = h; }

// void initTempHum() {
//     pinMode(PIN_TEMP_HUM, INPUT);
//     Serial.println("XHT11 init done");
// }

// TempHumData readTempHum() {
//   TempHumData data{};

//   if (simEnabled) {
//     data.temp = simTemp;
//     data.hum  = simHum;   // ja, “fake”, aber ok für Demo
//     return data;
//   }

//   if (sensor_xht.receive(raw)) {
//     data.hum  = raw[0];
//     data.temp = raw[2];
//   } else {
//     // Fehlerkennzeichnung (dein Ansatz ist ok)
//     data.hum  = -1;
//     data.temp = -100;
//   }
//   return data;
// }

// // TempHumData readTempHum() {
// //     TempHumData data;

// //     if (sensor_xht.receive(rawData)) {
// //         // wie im Demo:
// //         // buf[0] = Humidity (int),
// //         // buf[2] = Temperature (int)
// //         data.hum  = rawData[0];
// //         data.temp = rawData[2];
// //     } else {
// //         data.hum  = -1;
// //         data.temp = -100;   // „offensichtlich falsch“
// //     }

// //     return data;
// // }
