#include <Wire.h>
#include "hardware.h"
#include "MFRC522_I2C.h"
#include "mod_rfid.h"

MFRC522 mfrc522(0x28);
static RfidCallback rfidCallback = nullptr;
static String lastUid = "";

void registerRfidCallback(RfidCallback cb) {
  rfidCallback = cb;
}

void initRFID() {
  Wire.begin();
  mfrc522.PCD_Init();
}

static String readUidString(const MFRC522::Uid& uid) {
    String s;
    for (byte i = 0; i < uid.size; i++) {
        if (uid.uidByte[i] < 0x10) s += "0";
        s += String(uid.uidByte[i], HEX);
    }
    s.toUpperCase();
    return s;
}

void loopRfid() {
    if (!mfrc522.PICC_IsNewCardPresent()) return;
    if (!mfrc522.PICC_ReadCardSerial())   return;

    String uid = readUidString(mfrc522.uid);
    // entprellen: gleiche Karte ignorieren
    if (uid == lastUid) return;
    lastUid = uid;

    Serial.print("RFID UID: ");
    Serial.println(uid);

    if (rfidCallback) rfidCallback(uid);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
}

