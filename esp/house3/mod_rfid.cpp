#include <Wire.h>
#include "MFRC522_I2C.h"
#include "mod_rfid.h"
#include "mod_partylogic.h"
#include "mod_lcd.h"
#include "mod_songs.h"

// RFID-Adresse, Objekt
MFRC522 mfrc522(0x28);

void initRFID() {
  Wire.begin();
  mfrc522.PCD_Init();
}

void rfidLoop() {
  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial())   return;

  String uidHex = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) uidHex += "0";
    uidHex += String(mfrc522.uid.uidByte[i], HEX);
  }
  uidHex.toUpperCase();

  if (currentMode != MODE_PARTY) {
    printTempLcd("NO PARTY", "NO SONG", 3000);
  } else {
    SongId s = getSongForTag(uidHex);
    if (s == SongId::NONE) {
      printTempLcd("UNBEKANNT", "KEIN SONG", 3000);
    } else {
      printTempLcd("RFID OK", "Song startet", 2000);
      playSong(s);
    }
  }

  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
