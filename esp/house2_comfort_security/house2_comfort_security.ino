// Haus 2 - Gäste-Komfort & Sicherheit (Christoph)
//
// Zweck: Zutritt, Statusanzeige, Bedienung.
// Module: RFID, Buttons, LCD, RGB-LED, Servo.
// RFID ok → „belegt“, Servo öffnet Fenster/Rollo, Tür dreht
// Button für Checkout → „Leerstand“. LCD Display: Anzeige des Belegungstatus
//
// Gemeinsame Szenarien:
// 1) Sturmmodus → Servo schließt, gelbe LED auf Warnzustand, Buzzer
// 2) Gasalarm -> LED blinkt rot
// 3) Party-Modus -> Wird Party gestarted, wird Fenster geschlossen

// Setup:
// Change baud speed to 115200

// Load Libraries
#include <Arduino.h>
#include "config.h"
#include "hardware.h"
#include "mod_wifi.h"
#include "mod_lcd.h"
#include "mod_rfid.h"
#include "mod_window.h"
#include "mod_door.h"
#include "mod_mqtt.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initLcd();
  initWiFi();
  initMqtt();
  initRFID();
  initWindow();
  initDoor();
}

void loop() {
  // put your main code here, to run repeatedly:
  wiFiLoop();
  loopMqtt();
  lcdUpdate();
  loopRFID();
}
