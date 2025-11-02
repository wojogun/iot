# Projekt-Szenario: Smart Beach Resort am niederländischen Strand

Unser Projekt umfasst vier intelligente Ferienhäuser, die in unmittelbarer Nähe zum niederländischen Meeresstrand stehen und über Airbnb vermietet werden. Die besonderen Herausforderungen dieser Lage sind die hohe Luftfeuchtigkeit, Sandstürme, Salzwasser-Korrosion und die Notwendigkeit einer effizienten Ferienhaus-Verwaltung.

## Individuelle Use Cases

### Haus 1: Wetter & Sturm-Überwachung
Zweck: Umweltlage erfassen und Resort-Alarme auslösen.
Module: Temp/Feuchte, Steam-Sensor, PIR, Buzzer, LCD.
- Hohe Feuchte + Windwarnung (über Dashboard simuliert) → „Sturmmodus“ an alle.
- PIR bei Leerstand → Sicherheitsalarm über RGB-LED (rot).  
LCD Display: Anzeige der aktuellen Wetterdaten
Gemeinsam: Sturmmodus → Servo schließt, gelbe LED auf Warnzustand, Buzzer

### Haus 2 - Gäste-Komfort & Sicherheit
Zweck: Zutritt, Statusanzeige, Bedienung.
Module: RFID, Buttons, LCD, RGB-LED, Servo.
- RFID ok → „belegt“, Servo öffnet Fenster/Rollo, Tür dreht
- Button für Checkout → „Leerstand“.
LCD Display: Anzeige des Belegungstatus
Gemeinsam: Sturmmodus → Servo schließt, gelbe LED auf Warnzustand, Buzzer

### Haus 3 - Partyhaus
Zweck: Musik, Lichtspiele für junge und junggebliebene Menschen. Party über Dashboard buchbar.
- RGB-Modul für Lichtspiele
- Buzzer für Melodien
LCD Display: Anzeige der nächsten Party
Gemeinsam: Sturmmodus → Servo schließt, gelbe LED auf Warnzustand, Buzzer.

### Haus 4 -Luftqualität & Automatische Lüftung
Zweck: Es soll automatisch für frische Luft und sichere Raumluftqualität gesorgt werden. Der Gas-Sensor erkennt erhöhte Konzentrationen (CO, H₂ o.ä.), das System reagiert mit automatischer Lüftung und meldet den Vorfall ans Dashboard. - Gas anormal → Sofortalarm + Lüften anfordern.
- Gas Sensor
- RGB-Modul für Warnanzeige
- Button Module 1: Rücksetzung
LCD Display: Luftwerte werden angezeigt, ggf. Warnung und Lüfterstatus.
Gemeinsam: Sturmmodus → Servo schließt, gelbe LED auf Warnzustand, Buzzer.

## Gemeinsamer Use Case: Smart Beach Resort Management
Alle vier Häuser bilden zusammen ein intelligentes Ferienhaus-Ökosystem, das sowohl über lokale Bluetooth-Kommunikation als auch über individuelle Handy-Hotspots mit dem Internet verbunden ist. Ein zentrales Dashboard ermöglicht es dem Airbnb-Host, alle vier Häuser gleichzeitig zu überwachen und zu verwalten. Bei Sturmwarnungen werden automatisch alle Fenster in allen Häusern geschlossen und die Lüftung wird deaktiviert, um Sand und Sturm fernzuhalten. 
