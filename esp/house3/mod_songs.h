#ifndef MOD_SONGS_H
#define MOD_SONGS_H

#include <Arduino.h>
#include "hardware.h"

// Song-IDs, die in der Partylogik oder beim RFID-Lesen genutzt werden
enum class SongId {
    NONE = 0,
    SONG1,
    SONG2,
    SONG3,
    SONG4,
    SONG5
};

// Zuordnung UID → Song
SongId getSongForTag(const String& uid);
const char* songName(SongId id); 

// Abspielen eines Songs (wird später mit deiner Audio/Buzzer-Logik gefüllt)
void playSong(SongId id);
void sendMqttSongName(SongId id);

void smokeOnTheWater();
void werHatAnDerUhrGedreht();
void getThePartyStarted();
void whatShallWeDo();
void finalCountdown();
void heyJude();

#endif
