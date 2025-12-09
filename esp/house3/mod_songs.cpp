#include "config.h"
#include "mod_songs.h"
#include "mod_mqtt.h"
#include "mod_lcd.h"

void smokeOnTheWater();
void werHatAnDerUhrGedreht();
void getThePartyStarted();
void whatShallWeDo();
void finalCountdown();
void heyJude();

const SongInfo songList[] = {
    { 0, "kein Song" , "", nullptr },
    { 1, "Hey Jude",              "747B9FDB",       heyJude },              // karte
    { 2, "Final Countdown",       "9B1C5D8A",       finalCountdown },       // blau
    { 3, "Smoke on the Water",    "0462852A503880", smokeOnTheWater },      // TU-Card
    { 4, "Drunken Sailor",        "64000E7B",       whatShallWeDo },        // ORF
    { 5, "Paulchen Panther",      "023C16F0040020", werHatAnDerUhrGedreht }, // Visa
    { 6, "Get the Party started", "",               getThePartyStarted }
};

const uint8_t SONG_COUNT = sizeof(songList) / sizeof(songList[0]);

// generische Song-Suche
const SongInfo* getSongById(uint8_t id) {
    for (uint8_t i = 0; i < SONG_COUNT; i++) {
        if (songList[i].id == id) return &songList[i];
    }
    return 0;
}

const SongInfo* getSongByRfid(const String& rfid) {
    for (uint8_t i = 0; i < SONG_COUNT; i++) {
        if (rfid == songList[i].rfid)
            return &songList[i];
    }
    return nullptr;
}

void playSong(uint8_t id) {
    const SongInfo* s = getSongById(id);
    if (!s) {
        return;
    }

    // LCD-Update, MQTT und dann los
    printTempLcd("Song:", s->name, 5000);
    publishMqtt(TOPIC_CURRENT_SONG, s->name);
    if (id > 0 && id < SONG_COUNT) s->play();
}

void handleRfidSong(const String& uid) {
    const SongInfo* s = getSongByRfid(uid);
    playSong(s->id);
}

void publishSongList() {
    String json = "[";
    for (uint8_t i = 1; i < SONG_COUNT; i++) {
        const SongInfo& s = songList[i];
        if (i > 1) json += ",";
        json += "{\"id\":" + String(s.id) + ",\"name\":\"" + s.name + "\"}";
    }
    json += "]";
    publishMqtt(TOPIC_SONGLIST, json, true);  // retained, damit Node-RED sofort was hat
}

const float STACCATO = 0.85f;
void playNote(int freq, int baseLenMs) {
  int d = (int)(baseLenMs * STACCATO);
  int p = baseLenMs - d;
  buzzer.playTone(freq, d);
  if (p > 0) delay(p);
}

// ======================== SONGS ==============================
void smokeOnTheWater() {
  int BPM    = 112;
  int Q      = 60000 / BPM;   // Viertel
  int E      = Q / 2;         // Achtel
  int S      = Q / 4;         // Sechzehntel

  buzzer.playTone(392, Q);   // G4
  buzzer.playTone(466, Q);   // Bb4
  buzzer.playTone(523, Q+E);   // C5 (lang)
  buzzer.playTone(392, E);   // G4
  delay(E);
  buzzer.playTone(466, E);
  delay(E);
  buzzer.playTone(554, E);
  buzzer.playTone(523, Q+Q);
  buzzer.playTone(392, Q);
  buzzer.playTone(466, Q);
  buzzer.playTone(523, Q+E);
  buzzer.playTone(466, E);
  delay(E);
  buzzer.playTone(392, E+3*Q);   // Abschlussnote (lang)

  buzzer.playTone(0, 0);     // aus
}

void werHatAnDerUhrGedreht() {
  int BPM    = 122;
  int Q      = 60000 / BPM;   // Viertel
  int E      = Q / 2;         // Achtel
  int S      = Q / 4;         // Sechzehntel

  buzzer.playTone(587, Q);   // D5
  delay(E);
  playNote(440, E);   // A4 
  playNote(392, Q);   // G4 
  playNote(370, Q);   // F#4 

  playNote(330, Q);   // E4 
  playNote(494, Q);   // H4 
  playNote(494, Q);   // H4 
  delay(Q);

  playNote(554, Q);   // C#5
  playNote(494, Q);   // H4 
  playNote(440, Q);   // A4 
  playNote(392, Q);   // G4 

  playNote(370, Q);   // F#4
  playNote(494, Q);   // H4  
  playNote(440, Q);   // A4 
  delay(Q);
}

void getThePartyStarted() {
  int BPM    = 128;
  int Q      = 60000 / BPM;   // Viertel
  int E      = Q / 2;         // Achtel
  int S      = Q / 4;         // Sechzehntel

  delay(Q);
  delay(E);
  buzzer.playTone(370, E);   // F#4
  buzzer.playTone(370, E);   // F#4
  playNote(370, E);   // F#4
  playNote(330, E);   // E4
  playNote(294, E);   // D4

  playNote(330, E);   // E4
  playNote(247, E);   // H3
  playNote(247, E);   // H3
  playNote(247, E);   // H3
  playNote(294, E);   // D4
  playNote(294, E);   // D4
  playNote(392, E);   // G4
  buzzer.playTone(370, E);   // F#4

  playNote(370, E);   // F#4
  delay(E);
  buzzer.playTone(370, E);   // F#4
  buzzer.playTone(370, E);   // F#4
  playNote(370, E);   // F#4
  playNote(330, E);   // E4
  playNote(294, E);   // D4

  playNote(330, E);   // E3
  playNote(294, E);   // D4
  delay(E);
  playNote(294, E);   // D4
  playNote(330, E);   // E3
  playNote(294, E);   // D4
  playNote(247, E);   // H3
  playNote(247, E);   // H3
}

void whatShallWeDo() {
  int BPM    = 112;
  int Q      = 60000 / BPM;   // Viertel
  int E      = Q / 2;         // Achtel
  int S      = Q / 4;         // Sechzehntel

  playNote(440, E);   // A4
  playNote(440, S);   // A4
  playNote(440, S);   // A4
  playNote(440, E);   // A4
  playNote(440, S);   // A4
  playNote(440, S);   // A4
  
  playNote(440, E);   // A4
  playNote(294, E);   // D4
  playNote(349, E);   // F4
  playNote(440, E);   // A4
  
  playNote(392, E);   // G4
  playNote(392, S);   // G4
  playNote(392, S);   // G4
  playNote(392, E);   // G4
  playNote(392, S);   // G4
  playNote(392, S);   // G4

  playNote(392, E);   // G4
  playNote(262, E);   // C4
  playNote(330, E);   // E4
  playNote(392, E);   // G4

  playNote(440, E);   // A4
  playNote(440, S);   // A4
  playNote(440, S);   // A4
  playNote(440, E);   // A4
  playNote(440, S);   // A4
  playNote(440, S);   // A4
  
  playNote(440, E);   // A4
  playNote(494, E);   // H4
  playNote(523, E);   // C5
  playNote(587, E);   // D5

  playNote(523, E);   // C5
  playNote(440, E);   // A4
  playNote(392, E);   // G4
  playNote(330, E);   // E4

  playNote(294, Q);   // D4
  playNote(294, Q);   // D4
}

void finalCountdown() {
  int BPM    = 112;
  int Q      = 60000 / BPM;   // Viertel
  int H      = Q*2;           // Halbe
  int E      = Q / 2;         // Achtel
  int S      = Q / 4;         // Sechzehntel
  
  delay(Q+E);
  playNote(554, S);   // C#5
  playNote(494, S);   // H4
  playNote(554, Q);   // C#5
  buzzer.playTone(370, Q);   // F#5

  buzzer.playTone(370, Q);   // F#5
  delay(E);
  playNote(587, S);   // D5
  playNote(554, S);   // C#5
  playNote(587, E);   // D5
  playNote(554, E);   // C#5
  buzzer.playTone(494, Q);   // H4

  buzzer.playTone(494, Q);   // H4
  delay(E);
  playNote(587, S);   // D5
  playNote(554, S);   // C#5
  playNote(587, Q);   // D5
  buzzer.playTone(370, Q);   // F#4

  buzzer.playTone(370, Q);   // F#4
  delay(E);
  playNote(494, S);   // H4
  playNote(440, S);   // A4
  playNote(494, E);   // H4
  playNote(440, E);   // A4
  playNote(415, E);   // G#4
  playNote(494, E);   // H4

  playNote(440, Q+E);   // A4
  playNote(415, S);   // G#4
  playNote(440, S);   // A4
  playNote(494, Q+E);   // H4
  playNote(440, S);   // A4
  playNote(494, S);   // H4
  
  playNote(554, E);   // C#5
  playNote(494, E);   // H4
  playNote(440, E);   // A4
  playNote(415, E);   // G#4
  playNote(370, Q);   // F#4
  playNote(587, Q);   // D5

  playNote(554, Q+Q+Q);   // C#5
  playNote(554, S);   // C#5
  playNote(587, S);   // D5
  playNote(554, S);   // C#5
  playNote(494, S);   // H4
 
  playNote(554, Q);   // C#5
}

void heyJude() {
  int BPM    = 88;
  int Q      = 60000 / BPM;   // Viertel
  int H      = Q*2;           // Halbe
  int E      = Q / 2;         // Achtel
  int S      = Q / 4;         // Sechzehntel
  
  playNote(523, Q);   // C5

  buzzer.playTone(440, H);   // A4
  playNote(440, E);   // A4
  playNote(440, E);   // A4
  playNote(523, E);   // C5
  playNote(587, E);   // D5

  playNote(392, H+Q);   // G4
  playNote(392, E);   // G4
  playNote(440, E);   // A4

  playNote(466, Q);   // Hb4
  buzzer.playTone(698, Q);   // F5
  playNote(698, E);   // F5
  playNote(698, E);   // F5
  playNote(659, E);   // E5
  playNote(523, E);   // C5

  playNote(587, E);   // D5
  playNote(523, S);   // C5
  playNote(466, S);   // Hb4
  playNote(440, H);   // A4
  delay(E);
  playNote(523, E);   // C5

  playNote(587, E);   // D5
  playNote(587, Q);   // D5
  playNote(587, E);   // D5
  playNote(784, E);   // G5
  playNote(698, S);   // F5
  buzzer.playTone(659, S);   // E5
  playNote(659, S);   // E5
  playNote(698, S);   // F5
  playNote(587, E);   // D5

  playNote(523, H);   // C5
  playNote(349, E);   // F4
  playNote(392, E);   // G4
  playNote(440, E);   // A4
  buzzer.playTone(587, E);   // D5

  playNote(523, Q);   // C5
  delay(E);
  playNote(523, E);   // C5
  playNote(466, E);   // Hb4
  playNote(440, E);   // A4
  playNote(330, E);   // E4
  playNote(349, E);   // F4

  playNote(349, H+Q);   // F4
}