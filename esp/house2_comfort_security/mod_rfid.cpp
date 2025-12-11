#include <Wire.h>
#include "MFRC522_I2C.h"
#include "config.h"
#include "mod_window.h"
#include "mod_door.h"
#include "mod_lcd.h"
#include "mod_mqtt.h"

// IIC pins default to GPIO21 and GPIO22 of ESP32
// 0x28 is the i2c address of SDA, if doesn't match，please check your address with i2c.
MFRC522 mfrc522(0x28);   // create MFRC522.
#define btnPin 16
boolean btnFlag = 0;

String password = "";

void initRFID() {
  Serial.begin(115200);           // initialize and PC's serial communication
  Wire.begin();                   // initialize I2C
  mfrc522.PCD_Init();             // initialize MFRC522
  Serial.println(F("Scan PICC to see UID, type, and data blocks..."));
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
        publishMqtt(TOPIC_STATUS_HOUSE4, "LEER");
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
    //Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(mfrc522.uid.uidByte[i]);
    password = password + String(mfrc522.uid.uidByte[i]);
  }
  if(password == RFID_KEY)  // if Card number is correct, open the door and window
  {
    Serial.println("open");
    printLcd("Status:", "Belegt", false);
    publishMqtt(TOPIC_STATUS_HOUSE4, "BELEGT");
    openWindow();
    openDoor();
    password = "";
    btnFlag = 1;
  }
  else   //if Card number error, dispaly error
  {
    password = "";
    printLcd("Fehler:", "Falscher Schlüssel", false);
    publishMqtt(TOPIC_STATUS_HOUSE4, "Falscher Schlüssel");
  }
  //Serial.println(password);
}
