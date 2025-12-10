#include <Arduino.h>
#include <ESP32_Display_Panel.hpp>

#include "config.h"
#include "mod_wifi.h"
#include "mod_mqtt.h"

using namespace EDP;
Panel_ST7701 lcd;//#include "TinyMqtt.h"
const uint8_t  RETAIN = 10;  // Max retained messages

//MqttBroker broker(MQTT_LOCALPORT, MQTT_LOCALRETAIN);

void setup() {
  Serial.begin(115200);

  lcd.begin();
  lcd.setBrightness(255); // Backlight auf 100 %
  lcd.fillScreen(lcd.color565(0, 50, 200)); // Testfarbe anzeigen

  initWiFi();
  //initMqtt();
  //broker.begin(MQTT_LOCALPORT);
  Serial.print("Local MQTT broker started on port ");
  Serial.println( String(MQTT_LOCALPORT) );
}

void loop() {
  loopWiFi();
  //broker.loop();      // Broker bedienen
  //loopMqtt();         // eigener Client (für UI)
  //uiLoop();           // LVGL
}


/*
USED LIBS
- GFX_Library_for_Arduino	 1.6.3
- lvgl 9.4.0
- sensorlib 0.3.3
- XPowersLib 0.3.2
*/