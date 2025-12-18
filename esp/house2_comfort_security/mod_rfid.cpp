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

void initRFID() {
  Serial.begin(115200);           // initialize and PC's serial communication
  Wire.begin();                   // initialize I2C
  mfrc522.PCD_Init();             // initialize MFRC522
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
}

void initRuntimeConfig() {
  prefs.begin("house2", false); // RW namespace
  String saved = prefs.getString("rfid_key", "");
  g_rfidKey = saved.length() ? saved : String(RFID_KEY_DEFAULT);
  Serial.println("RFID key loaded: " + g_rfidKey);
}

String getRfidKey() {
  return g_rfidKey;
}

bool setRfidKey(const String& key) {
  g_rfidKey = key;
  size_t written = prefs.putString("rfid_key", key);
  Serial.println(String("RFID key updated (") + (written > 0 ? "saved" : "not saved") + "): " + g_rfidKey);
  return written > 0;
}

void loopRFID() {
  // 
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    password = "";
    if(btnFlag == 1)
    {
      boolean btnVal = digitalRead(btnPin);
      if(btnVal == 0)  //If door close button is pressed (active-low)
      {
        Serial.println("close");
        printLcd("Status:", "Leerstand", false);
        publishMqtt(TOPIC_STATUS_HOUSE2, "LEER");
        closeWindow();
        closeDoor();
        btnFlag = 0;
      }
    }
    return;
  }
  
  // select one of door cards. UID and SAK are mfrc522.uid.
  
  // save UID
  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i]);
    password = password + String(mfrc522.uid.uidByte[i]);
  }

  // Compare with runtime key (from NVS or default)
  if (password == getRfidKey())
  {
    Serial.println("open");
    printLcd("Status:", "Belegt", false);
    publishMqtt(TOPIC_STATUS_HOUSE2, "BELEGT");
    openWindow();
    openDoor();
    password = "";
    btnFlag = 1;
  }
  else
  {
    password = "";
    printLcd("Fehler:", "Falscher Key", false);
    publishMqtt(TOPIC_STATUS_HOUSE2, "Falscher Key");
  }
  //Serial.println(password);
}
