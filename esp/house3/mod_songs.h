#ifndef MOD_SONGS_H
#define MOD_SONGS_H

#include <Arduino.h>
#include "hardware.h"

typedef void (*SongPlayCallback)();
struct SongInfo {
    uint8_t id;              // 1..N
    const char* name;        // Anzeigename
    const char* rfid;        // RFID-Tag
    SongPlayCallback play;   // Funktion, die den Song abspielt
};

extern const SongInfo songList[];
extern const uint8_t SONG_COUNT;

const SongInfo* getSongById(uint8_t id);
const SongInfo* getSongByRfid(const String& rfid);

void handleRfidSong(const String& uid);
void playSong(uint8_t id);
void publishSongList();
void playNote(int freq, int baseLenMs, float staccato = 0.85f);
#endif
