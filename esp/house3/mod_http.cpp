#include <WiFi.h>
#include "mod_partylogic.h"
#include "mod_songs.h"

static WiFiServer httpServer(80);
static String lastStatusMessage;

static String modeToText() {
    switch (currentMode) {
        case MODE_PARTY: return "PARTY";
        case MODE_STORM: return "STORM";
        case MODE_NORMAL:
        default:         return "NORMAL";
    }
}

static String extractPath(const String& requestLine) {
    int s1 = requestLine.indexOf(' ');
    if (s1 < 0) return "/";
    int s2 = requestLine.indexOf(' ', s1 + 1);
    if (s2 < 0) s2 = requestLine.length();
    return requestLine.substring(s1 + 1, s2);
}

static String buildRootPage(const String& ipStr, const String& modeText, const String& statusMessage) {
  String s;
  s.reserve(2048);

  s += "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html; charset=utf-8\r\n";
  s += "Connection: close\r\n\r\n";

  s += "<!DOCTYPE html><html><head><meta charset='utf-8'>";
  s += "<title>Haus 3 – Partyhaus</title>";
  s += "<style>";
  s += "body{font-family:Arial,Helvetica,sans-serif;margin:20px;}";
  s += "h1{margin-bottom:5px;}";
  s += "p{margin:5px 0;}";
  s += "button{padding:10px 20px;margin:5px;border-radius:6px;border:1px solid #444;cursor:pointer;}";
  s += "button:hover{background:#eee;}";
  s += ".btn-party{background:#ffd54f;}";
  s += ".btn-storm{background:#ff8a80;}";
  s += ".btn-song{background:#00d5d5;}";
  s += "footer{margin-top:25px;color:#666;font-size:0.9em;border-top:1px solid #ddd;padding-top:8px;}";
  s += "</style>";
  s += "</head><body>";

  s += "<h1>Haus 3 – Partyhaus</h1>";
  s += "<p><b>IP:</b> " + ipStr + "</p>";
  s += "<p><b>Modus:</b> " + modeText + "</p>";

  // Buttons
  s += "<p>";
  s += "<a href='/party/start'><button class='btn-party'>Party START</button></a>";
  s += "<a href='/party/stop'><button class='btn-party'>Party STOP</button></a>";
  s += "</p>";

  s += "<p>";
  s += "<a href='/storm/on'><button class='btn-storm'>STURM EIN</button></a>";
  s += "<a href='/storm/off'><button class='btn-storm'>STURM AUS</button></a>";
  s += "</p>";
  s += "<hr />";
  s += "<p>";
  s += "<h1>Wurlitzer-Test</h1>";
  s += "<a href='/?song=1'><button class='btn-song'>" + String(songName(SongId::SONG1)) + "</button></a><br />";
  s += "<a href='/?song=2'><button class='btn-song'>" + String(songName(SongId::SONG2)) + "</button></a><br />";
  s += "<a href='/?song=3'><button class='btn-song'>" + String(songName(SongId::SONG3)) + "</button></a><br />";
  s += "<a href='/?song=4'><button class='btn-song'>" + String(songName(SongId::SONG4)) + "</button></a><br />";
  s += "<a href='/?song=5'><button class='btn-song'>" + String(songName(SongId::SONG5)) + "</button></a><br />";
  s += "</p>";
  
  // Fußzeile / Feedback
  s += "<footer>";
  if (statusMessage.length() > 0) {
    s += "Status: " + statusMessage;
  } else {
    s += "Bereit.";
  }
  s += "</footer>";

  s += "</body></html>";

  return s;
}

void initHttp() {
    httpServer.begin();
    Serial.println("HTTP server started on port 80");
}

void loopHttp() {
    WiFiClient client = httpServer.available();
    if (!client) return;

    client.setTimeout(1000);

    // Erste Zeile lesen: "GET /... HTTP/1.1"
    String requestLine = client.readStringUntil('\r');
    client.readStringUntil('\n'); // Rest der Zeile wegwerfen

    // Restliche Header bis zur Leerzeile verwerfen
    while (client.connected()) {
        String line = client.readStringUntil('\r');
        client.readStringUntil('\n');
        if (line.length() == 0) break; // leere Zeile => Ende Header
    }

    String path = extractPath(requestLine);
    lastStatusMessage = "";

    // Hier je nach Pfad Aktionen auslösen – an deine HTML-Links anpassen!
    if (path.indexOf("party=on") >= 0) {
        startParty(true);
        lastStatusMessage = "Party gestartet";
    } else if (path.indexOf("party=off") >= 0) {
        stopParty(true);
        lastStatusMessage = "Party gestoppt";
    } else if (path.indexOf("storm=on") >= 0) {
        startStorm(true);
        lastStatusMessage = "Sturm gestartet";
    } else if (path.indexOf("storm=off") >= 0) {
        stopStorm(true);
        lastStatusMessage = "Sturm gestoppt";

    } else if (path.indexOf("song=1") >= 0) {
        playSong(SongId::SONG1);
        lastStatusMessage = String("Song: ") + songName(SongId::SONG1);
    } else if (path.indexOf("song=2") >= 0) {
        playSong(SongId::SONG2);
        lastStatusMessage = String("Song: ") + songName(SongId::SONG2);
    } else if (path.indexOf("song=3") >= 0) {
        playSong(SongId::SONG3);
        lastStatusMessage = String("Song: ") + songName(SongId::SONG3);
    } else if (path.indexOf("song=4") >= 0) {
        playSong(SongId::SONG4);
        lastStatusMessage = String("Song: ") + songName(SongId::SONG4);
    } else if (path.indexOf("song=0") >= 0) {
        // Song stoppen / NONE setzen
        sendMqttSongName(SongId::NONE);
        lastStatusMessage = "Song gestoppt";
    } else {
        // nur Seite neu zeichnen
        lastStatusMessage = "";
    }

    String ipStr = WiFi.localIP().toString();
    String modeStr = modeToText();
    String response = buildRootPage(ipStr, modeStr, lastStatusMessage);

    client.print(response);
    client.stop();
}


