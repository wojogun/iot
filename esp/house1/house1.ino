/************************************************************
 * Haus 1 – Weather & Storm (Lisa)
 ************************************************************/
#include <Arduino.h>
#include "config.h"
#include "hardware.h"
#include "mod_config_wifi.h"
#include "mod_config_mqtt.h"
#include "mod_logic_common.h"
#include "mod_logic_storm.h"
#include "mod_logic_weather.h"
#include "mod_mqtt_house1.h"
#include "mod_sensor_button.h"
#include "mod_sensor_lcd.h"
#include "mod_weather.h"

void setup()
{
  Serial.begin(115200);
  initHardware();
  initLcd();
  initWiFi();
  initMqtt();           // verbindet
  initMqttHouse1();     // registriert callback + subscribes
  initCommon();         // nur publish initial gas/party
  initStormLogic();     // publish Status initial (storm)

  // Haus 1 Weather Logic
  initWeather(); // Temp/Hum/Steam/Wind
  initWeatherLogic();

    initButtons();      // lokale buttons initial
}

void loop()
{
  wiFiLoop();
  mqttLoop();
  loopMqttHouse1();
  lcdUpdate();            // Display aktualisieren
  loopYellowLed();
  loopRgb();
  loopButtons();
  loopWeatherLogic();     // ruft intern readWeather + loopStormLogic(w)
}
