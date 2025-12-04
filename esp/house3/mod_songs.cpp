#include "mod_songs.h"
#include "mod_mqtt.h"

// Beispiel: feste Zuordnung von RFID-UIDs zu Songs
// Die UID-Strings müssen genau zu den Hex-Strings aus mod_rfid.cpp passen.
struct TagSongMap {
    const char* uid;
    SongId      song;
};

// Beispielhafte Tabelle – bitte an deine echten Tags anpassen!
static const TagSongMap SONG_MAP[] = {
    { "747B9FDB", SongId::SONG1 }, // karte
    { "9B1C5D8A", SongId::SONG2 }, // blau
    { "8899AA77", SongId::SONG3 },
    { "DEADBEEF", SongId::SONG4 }
};

static const size_t SONG_MAP_SIZE = sizeof(SONG_MAP) / sizeof(SONG_MAP[0]);

SongId getSongForTag(const String& uid) {
    for (size_t i = 0; i < SONG_MAP_SIZE; i++) {
        if (uid.equalsIgnoreCase(SONG_MAP[i].uid)) {
            return SONG_MAP[i].song;
        }
    }
    return SongId::NONE;
}

const char* songName(SongId id) {
    switch (id) {
        case SongId::SONG1: return "Hey Jude";
        case SongId::SONG2: return "Final Countdown";
        case SongId::SONG3: return "Smoke on the Water";
        case SongId::SONG4: return "What shall we do";
        case SongId::SONG5: return "wer hat an der Uhr gedreht";
        case SongId::NONE:
        default:
            return "NONE";
    }
}

void sendMqttSongName(SongId id) {
    auto& mqttClient = GetMqttClient();

    if (mqttClient.connected()) {
        mqttClient.publish(
            TOPIC_CURRENT_SONG,
            songName(id),
            true   // retained
        );
    }
}

void playSong(SongId id) {
    sendMqttSongName(id);
    switch(id) {
        case SongId::SONG1:
            Serial.println("Play Song 1");
            heyJude();
            break;

        case SongId::SONG2:
            Serial.println("Play Song 2");
            finalCountdown();
			break;

        case SongId::SONG3:
            Serial.println("Play Song 3");
            smokeOnTheWater();
            break;

        case SongId::SONG4:
            Serial.println("Play Song 4");
            whatShallWeDo();
			break;
        case SongId::SONG5:
            Serial.println("Play Song 5");
            werHatAnDerUhrGedreht();
			break;
        default:
            Serial.println("No song for this tag.");
            break;
    }
}

const float STACCATO = 0.85f;
void playNote(int freq, int baseLenMs) {
  int d = (int)(baseLenMs * STACCATO);
  int p = baseLenMs - d;
  buzzer.playTone(freq, d);
  if (p > 0) delay(p);
}

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
  sendMqttSongName(SongId::NONE);
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
  sendMqttSongName(SongId::NONE);
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
  sendMqttSongName(SongId::NONE);
}

void whatShallWeDo() {
  int BPM    = 112;
  int Q      = 60000 / BPM;   // Viertel
  int E      = Q / 2;         // Achtel
  int S      = Q / 4;         // Sechzehntel

  playNote(440, E);   // A4
  playNote(440, S);   // A4
  playNote(440, S);   // A4
  playNote(440, S);   // A4
  playNote(440, E);   // A4
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
  playNote(262, E);   // E4
  playNote(392, E);   // G4
  delay(E);
  sendMqttSongName(SongId::NONE);
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
  sendMqttSongName(SongId::NONE);
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
  sendMqttSongName(SongId::NONE);
}