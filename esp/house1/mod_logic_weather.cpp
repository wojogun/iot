#include <Arduino.h>
#include "config.h"
#include "mod_config_mqtt.h"
#include "mod_logic_storm.h"
#include "mod_logic_weather.h"
#include "mod_weather.h"
#include "mod_sensor_lcd.h"

// ===== settings =====
static const unsigned long WEATHER_READ_MS   = 1000;  // Sensor read interval
static const unsigned long MQTT_PUBLISH_MS   = 1000;  // publish interval
static const unsigned long SCREEN_SWITCH_MS  = 3000;  // only in NORMAL

// ===== cached weather =====
static WeatherData cachedW{};
static bool hasWeather = false;
static unsigned long lastReadMs = 0;
static unsigned long lastPublishMs = 0;

// display rotate (nur NORMAL)
static unsigned long lastScreenSwitchMs = 0;
static bool showValuesScreen = false;

// helper: normal screens
static void showNormalTextScreen() {
  printLcd("Haus 1", "Wetter normal", false);
}

static void showNormalValuesScreen(const WeatherData& w) {
  String l1 = "T:" + String(w.temp, 1) + "C H:" + String((int)w.hum) + "%";
  String l2 = "R:" + String(w.steam) + " W:" + String(w.wind);
  printLcd(l1, l2, false);
}

void initWeatherLogic() {
  lastReadMs = 0;
  lastPublishMs = 0;
  lastScreenSwitchMs = 0;
  showValuesScreen = false;
  hasWeather = false;
  showNormalTextScreen();
}

void loopWeatherLogic() {
  const unsigned long now = millis();

  // 1) Read sensors only every WEATHER_READ_MS
  if (!hasWeather || (now - lastReadMs >= WEATHER_READ_MS)) {
    lastReadMs = now;
    cachedW = readWeather();
    hasWeather = true;
  }

  // 2) Storm decision ALWAYS uses cached values (stable)
  loopStormLogic(cachedW);

  // 3) Publish cached values only every MQTT_PUBLISH_MS
  if (now - lastPublishMs >= MQTT_PUBLISH_MS) {
    lastPublishMs = now;

    publishMqtt(TOPIC_STATUS_TEMP,  String(cachedW.temp, 1), true);
    publishMqtt(TOPIC_STATUS_HUM,   String(cachedW.hum, 0), true);
    publishMqtt(TOPIC_STATUS_STEAM, String(cachedW.steam), true);
    publishMqtt(TOPIC_STATUS_WIND,  String(cachedW.wind), true);
  }

  // 4) LCD rotation only when NORMAL (no warning / no storm)
  if (!isStormActive() && !isStormWarningActive()) {
    if (lastScreenSwitchMs == 0) {
      lastScreenSwitchMs = now;
      showValuesScreen = false;
      showNormalTextScreen();
    }

    if (now - lastScreenSwitchMs >= SCREEN_SWITCH_MS) {
      lastScreenSwitchMs = now;
      showValuesScreen = !showValuesScreen;

      if (showValuesScreen) showNormalValuesScreen(cachedW);
      else                  showNormalTextScreen();
    }
  } else {
    // stop normal rotation in warning/storm
    lastScreenSwitchMs = 0;
  }
}

// #include <Arduino.h>
// #include "config.h"
// #include "mod_config_mqtt.h"
// #include "mod_logic_storm.h"
// #include "mod_logic_weather.h"
// #include "mod_sensor_lcd.h"
// #include "mod_weather.h"

// // publish
// static unsigned long lastPublishMs = 0;

// // display rotate (nur NORMAL)
// static unsigned long lastScreenSwitchMs = 0;
// static bool showValuesScreen = false;
// static const unsigned long SCREEN_SWITCH_MS = 3000;

// static void showNormalTextScreen() {
//   printLcd("Haus 1", "Wetter normal", false);
// }

// static void showNormalValuesScreen(const WeatherData& w) {
//   // 16x2 friendly
//   String l1 = "T:" + String(w.temp, 1) + "C H:" + String((int)w.hum) + "%";
//   String l2 = "R:" + String(w.steam) + " W:" + String(w.wind);
//   printLcd(l1, l2, false);
// }


// void initWeatherLogic() {
//   lastPublishMs = 0;
//   lastScreenSwitchMs = 0;
//   showValuesScreen = false;
//   showNormalTextScreen();
// }

// void loopWeatherLogic() {
//   WeatherData w = readWeather();

//   // Storm decision
//   loopStormLogic(w);


//   // MQTT publish alle 1s
//   unsigned long now = millis();
//   if (now - lastPublishMs >= 1000) {
//     lastPublishMs = now;
//     publishMqtt(TOPIC_STATUS_TEMP,  String(w.temp, 1), true);
//     publishMqtt(TOPIC_STATUS_HUM,   String(w.hum, 0), true);
//     publishMqtt(TOPIC_STATUS_STEAM, String(w.steam), true);
//     publishMqtt(TOPIC_STATUS_WIND,  String(w.wind), true);
//   }

//   // Nur wenn wirklich NORMAL: im Display alle 3s wechseln
//   if (!isStormActive() && !isStormWarningActive()) {
//     if (lastScreenSwitchMs == 0) {
//       lastScreenSwitchMs = now;
//       showValuesScreen = false;
//       showNormalTextScreen();
//     }

//     if (now - lastScreenSwitchMs >= SCREEN_SWITCH_MS) {
//       lastScreenSwitchMs = now;
//       showValuesScreen = !showValuesScreen;

//       if (showValuesScreen) showNormalValuesScreen(w);
//       else                  showNormalTextScreen();
//     }
//   } else {
//     // wenn Warn/Storm aktiv ist, stoppen wir den Normal-Wechsel
//     lastScreenSwitchMs = 0;
//   }
// }

