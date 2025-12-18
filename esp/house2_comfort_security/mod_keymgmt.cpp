// This module handles RFID reader operations for house 2 comfort security system

// Includes necessary libraries and other modules
#include <Wire.h>
#include "MFRC522_I2C.h"
#include "config.h"
#include "mod_window.h"
#include "mod_door.h"
#include "mod_lcd.h"
#include "mod_mqtt.h"
#include <Preferences.h>

// IIC pins default to GPIO21 and GPIO22 of ESP32
// 0x28 is the i2c address of SDA, if doesn't match，please check your address with i2c.
MFRC522 mfrc522(0x28);   // create MFRC522.
#define btnPin 16
boolean btnFlag = 0;

String password = "";

static Preferences prefs;
static String g_rfidKey;

// RFID key name mapping
struct RfidKeyMapping {
  String uid;
  String name;
};

static const RfidKeyMapping RFID_KEY_NAMES[] = {
  { "27184202138", "Blue Chip" },
  { "11679151219", "White Card" },
  { "ABCDEF12", "Other" }
};

// Get RFID key name from UID
String getRfidKeyName(const String& uid) {
  for (const auto& mapping : RFID_KEY_NAMES) {
    if (mapping.uid == uid) {
      return mapping.name;
    }
  }
  return "other";
}

// Initialize button
void initButton() {
  pinMode(btnPin, INPUT_PULLUP);
  Serial.println("Button initialized on pin " + String(btnPin));
}

// Check button state and handle door close
void checkButton() {
  if (btnFlag == 1) {
    boolean btnVal = digitalRead(btnPin);
    if (btnVal == 0) { // If door close button is pressed (active-low)
      Serial.println("close");
      printLcd("Status:", "Leerstand", false);
      publishMqtt(TOPIC_STATUS_HOUSE2, "LEER");
      closeWindow();
      closeDoor();
      btnFlag = 0;
    }
  }
}

// Initialize RFID reader
void initRFID() {
  Serial.begin(115200);           // initialize and PC's serial communication
  Wire.begin();                   // initialize I2C
  mfrc522.PCD_Init();             // initialize MFRC522
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
}

// Initialize runtime config for RFID key
void initRuntimeConfig() {
  prefs.begin("house2", false); // RW namespace
  String saved = prefs.getString("rfid_key", "");
  g_rfidKey = saved.length() ? saved : String(RFID_KEY_DEFAULT);
  Serial.println("RFID key loaded: " + g_rfidKey);
}

// Get current RFID key
String getRfidKey() {
  return g_rfidKey;
}

// Set new RFID key
bool setRfidKey(const String& key) {
  g_rfidKey = key;
  size_t written = prefs.putString("rfid_key", key);
  Serial.println(String("RFID key updated (") + (written > 0 ? "saved" : "not saved") + "): " + g_rfidKey);
  // Publish the key name (not the value) to the status topic
  String keyName = getRfidKeyName(key);
  publishMqtt(TOPIC_STATUS_RFID_KEY, keyName.c_str());
  return written > 0;
}

// Main loop for RFID processing
void loopRFID() {
  // 
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    password = "";
    checkButton();
    return;
  }
  
  // Show UID on serial monitor
  Serial.println(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.println(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.println(mfrc522.uid.uidByte[i]);
    password = password + String(mfrc522.uid.uidByte[i]);
  }

  // Compare with runtime key (from NVS or default)
  if (password == getRfidKey())
  {
    Serial.println("open");
    printLcd("Status:", "Belegt", false);
    String keyName = getRfidKeyName(password);
    publishMqtt(TOPIC_STATUS_HOUSE2, "BELEGT");
    publishMqtt(TOPIC_STATUS_RFID_KEY, keyName.c_str());
    openWindow();
    openDoor();
    password = "";
    btnFlag = 1;
  }
  else
  {
    password = "";
    printLcd("Fehler:", "Falscher Key", false);
    //publishMqtt(TOPIC_STATUS_HOUSE2, "Falscher Key");
    publishMqtt(TOPIC_STATUS_RFID_KEY, "Falscher Key");
  }
  Serial.println(password);
}
