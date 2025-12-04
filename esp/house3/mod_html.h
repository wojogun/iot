#pragma once
#include <Arduino.h>

// Baut das komplette HTML-Dokument inkl. HTTP-Header
// ipStr      = z.B. "192.168.100.244"
// modeText   = z.B. "NORMAL", "PARTY", "STURM"
// statusMessage = Text für die Fußzeile ("Party gestartet", ...)
String buildRootPage(const String& ipStr, const String& modeText, const String& statusMessage) {
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
