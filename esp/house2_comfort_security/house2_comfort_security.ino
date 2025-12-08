// Haus 2 - Gäste-Komfort & Sicherheit (Christoph)
// Zweck: Zutritt, Statusanzeige, Bedienung. Module: RFID, Buttons, LCD, RGB-LED, Servo.
// RFID ok → „belegt“, Servo öffnet Fenster/Rollo, Tür dreht
// Button für Checkout → „Leerstand“. LCD Display: Anzeige des Belegungstatus
// Gemeinsam: Sturmmodus → Servo schließt, gelbe LED auf Warnzustand, Buzzer

// Setup:
// Change baud speed to 115200

// Load Libraries
#include <Arduino.h>
#include "config.h"
#include "mod_wifi.h"
#include "mod_lcd.h"

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  initLcd();
  initWiFi();
}

void loop() {
  // put your main code here, to run repeatedly:
  wiFiLoop();
  lcdUpdate();
}
