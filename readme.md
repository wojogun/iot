# keyestudio Smart Home

## Überblick Sensoren und Module
Verwendetes Modell: KS5009
![Hausübersicht Bauteile](img/Hausübersicht_Bauteile.jpg)

Es gibt Unterschiede zum Modell KS0085S, auf die sich das Online-Tutorial bezieht.

### Vergleich der beiden Modelle

| Komponente          | KS5009 | KS0085S |
|---------------------|--------|---------|
|Plus Control Board      |❌|✅|
|ESP32+ DevBoard         |✅|❌|
|Keyestudio Sensor Shield|❌|✅|
|White LED Module        |❌|✅|
|Yellow LED Module       |✅|✅|
|RGB LED Module          |✅|❌|
|Button Module           |✅✅|✅✅|
|Photocell Sensor        |❌|✅|
|PIR Motion Sensor       |✅|✅|
|Gas Sensor              |✅|✅|
|Relay Module            |❌|✅|
|Bluetooth HM20 Module   |integriert in ESP32+|✅|
|Passive Buzzer Module   |✅|✅|
|Motor/Fan               |✅|✅|
|Steam Sensor            |✅|✅|
|Servo Motor (Fenster)   |✅|✅|
|LCD Display             |✅|✅|
|Soil Huminity Sensor    |❌|✅|
|RFID Module             |✅|❌|
|Temp and Hum Sensor     |✅|❌|

ESP32PLUS ist ein universelles WLAN- und Bluetooth-Entwicklungsboard auf ESP32-Basis, integriert mit dem ESP32-WOROOM-32-Modul und kompatibel mit Arduino. Es verfügt über einen Hall-Sensor, High-Speed-SDIO/SPI, UART, I2S sowie I2C. Es mit dem freeRTOS-Betriebssystem.

## Projekt-Szenario: Smart Beach Resort am niederländischen Strand

Unser Projekt umfasst drei intelligente Ferienhäuser, die in unmittelbarer Nähe zum niederländischen Meeresstrand stehen und über Airbnb vermietet werden. Die besonderen Herausforderungen dieser Lage sind die hohe Luftfeuchtigkeit, Sandstürme, Salzwasser-Korrosion und die Notwendigkeit einer effizienten Ferienhaus-Verwaltung.

### Individuelle Use Cases

**Haus 1: Wetter & Sturm-Überwachung**
Das erste Haus fungiert als intelligente Wetterstation für das gesamte Resort. Es überwacht kontinuierlich Temperatur und Luftfeuchtigkeit, um frühzeitig auf Sturmgefahren und extreme Wetterbedingungen zu reagieren. Der integrierte Dampfsensor erkennt die hohe Feuchtigkeit der Meeresluft und warnt vor potenzieller Korrosion. Bei ungewöhnlichen Bewegungen durch den PIR-Sensor wird automatisch eine Sicherheitswarnung ausgelöst. Der Gas-Sensor überwacht die Konzentration von Verbrennungsgasen (CO, H₂) im Haus und der Buzzer alarmiert bei Sturmwarnungen oder Gas-Alarmen alle Bewohner des Resorts.

**Verwendete Sensoren und Module:**
- Temp and Hum Sensor (Temperatur- und Luftfeuchtigkeitsmessung)
- Steam Sensor (Feuchtigkeitserkennung für Korrosionsschutz)
- PIR Motion Sensor (Bewegungserkennung für Sicherheit)
- Gas Sensor (Überwachung von Verbrennungsgasen CO/H₂)
- Passive Buzzer (Alarmierung bei Sturmwarnungen und Gas-Alarmen)
- LCD Display (Anzeige der aktuellen Wetterdaten)

**Haus 2: Gäste-Komfort & Sicherheit**
Das zweite Haus ist auf maximalen Gäste-Komfort und Sicherheit ausgerichtet. Gäste erhalten RFID-Karten für kontaktlosen Zugang, wodurch der traditionelle Schlüsseltausch entfällt. Über Button-Module können Gäste direkt Service-Anfragen tätigen, während das LCD-Display wichtige Informationen zu Check-in und Check-out anzeigt. Die RGB-LED-Beleuchtung schafft eine entspannte Atmosphäre, und der Servo-Motor steuert automatisch die Fenster zum Schutz vor Sand und Sturm.

**Verwendete Sensoren und Module:**
- RFID Module (Kontaktloser Zugang für Gäste)
- Button Module (Service-Anfragen und Bedienung)
- LCD Display (Check-in/Check-out Informationen)
- RGB LED Module (Stimmungs-Beleuchtung)
- Servo Motor (Automatisches Fenster-Schließen für Sandschutz)
- Yellow LED Module (Zusätzliche Beleuchtung)

**Haus 3: Energie & Wartung**
Das dritte Haus konzentriert sich auf Energieeffizienz und Wartungsoptimierung. Der Motor mit Ventilator sorgt für kontinuierliche Lüftung und Luftzirkulation im Haus. Das LCD-Display zeigt den aktuellen Status der Haussysteme an, während der Buzzer bei Wartungsbedarf alarmiert. Button-Module ermöglichen es dem Reinigungspersonal, die abgeschlossene Hausreinigung zu melden, und der Gas-Sensor überwacht die Konzentration von Verbrennungsgasen für die Sicherheit der Gäste.

**Verwendete Sensoren und Module:**
- Motor/Fan (Lüftung und Luftzirkulation)
- Passive Buzzer (Wartungs-Alerts und Benachrichtigungen)
- Button Module (Reinigungs-Status und Bedienung)
- LCD Display (System-Status-Anzeige)
- Gas Sensor (Überwachung von Verbrennungsgasen für Gästesicherheit)

### Gemeinsamer Use Case: Smart Beach Resort Management

Alle drei Häuser bilden zusammen ein intelligentes Ferienhaus-Ökosystem, das sowohl über lokale Bluetooth-Kommunikation als auch über individuelle Handy-Hotspots mit dem Internet verbunden ist. Die Häuser stehen in einem Klassenzimmer, wodurch die Bluetooth-Kommunikation zwischen ihnen optimal funktioniert. Ein zentrales Dashboard ermöglicht es dem Airbnb-Host, alle drei Häuser gleichzeitig zu überwachen und zu verwalten. Bei Sturmwarnungen werden automatisch alle Fenster in allen Häusern geschlossen und die Lüftung wird deaktiviert, um Sand und Sturm fernzuhalten. Das System organisiert die Reinigung bei Gäste-Wechsel und überwacht kontinuierlich die Umgebungsbedingungen in allen Gebäuden. Gäste können über eine mobile App einchecken, Service anfordern und wichtige Resort-Informationen abrufen, während das System automatisch die Lüftung während eines Sturmes deaktiviert.
Bei längerem Leerstand aktiviert sich regelmäßige Lüftung.

**Verwendete Technologien und Module:**
- ESP32+ DevBoard (WiFi-Verbindung zu individuellen Handy-Hotspots + Bluetooth-Kommunikation)
- Integriertes Bluetooth (lokale Kommunikation zwischen Häusern im Klassenzimmer)
- Cloud-Services (Daten-Austausch über Internet für Host-Dashboard)
- MQTT-Protokoll (Daten-Streaming über Cloud-Broker)
- Web-Server (zentrales Dashboard für Host)
- Mobile App (Gäste-Interface)
- Alle Sensoren und Module der einzelnen Häuser (lokal über BT + zentral über Cloud koordiniert)

**Hybride Kommunikations-Architektur:**
- **Lokale BT-Kommunikation**: Echtzeit-Alerts zwischen Häusern (Sturmwarnung, Rollos synchronisieren)
- **Cloud-Kommunikation**: Host-Dashboard, Gäste-App, Daten-Logging
- **Klassenzimmer-Umgebung**: Optimale BT-Reichweite für direkte Haus-zu-Haus-Kommunikation

## Szenario der Gruppe B
Wir haben drei mittels Online-Buchungssystem mietbare Häuser an der dänischen Nordseeküste. Die Usecases sind dabei folgende:
### Haus 1: Wetter & Sturm-Überwachung
- Temp and Hum Sensor für Innenklima
- Steam Sensor für Feuchtigkeit (Salzwasser-Korrosion)
- PIR Motion Sensor für unerlaubte Zutritte
- Gas Sensor für Heizung/Gas-Überwachung
- Passive Buzzer für Sturmwarnungen
### Haus 2: Gäste-Komfort & Sicherheit
- RFID Module für Schlüssel-Ersatz (Gäste-Cards)
- Button Module für Service-Anfragen
- LCD Display für Check-in/Check-out Info
- RGB LED für Stimmungs-Beleuchtung
- Servo Motor für automatische Rollos (Sandschutz)
### Haus 3: Energie & Wartung
- Motor/Fan für Lüftung (Sand-Filter)
- Passive Buzzer für Wartungs-Alerts
- Button Module für Reinigungs-Status
- LCD Display für Energieverbrauch
- Gas Sensor für CO-Monitoring

### Gemeinsamer Use Case: Smart Beach Resort Management
Ferienhaus-Ökosystem:
- Zentrales Airbnb-Management Dashboard
- Automatische Check-in/Check-out (RFID-basiert)
- Wetter-Alert-System (Sturmwarnung für alle Gäste)
- Energie-Optimierung (Heizung basierend auf Belegung)
- Wartungs-Koordination (Reinigung zwischen Gästen)
- Sand-Korrosion-Überwachung (alle Häuser)
Technische Features:
- MQTT-Broker für Resort-Kommunikation
- Web-Dashboard für Airbnb-Host
- Mobile App für Gäste (Check-in, Service)
- Automatische Backup-Heizung bei Sturm
- Sand-Protection-System (automatische Rollos)
Business-Logic:
- Gäste bekommen RFID-Card für Zugang
- Automatische Beheizung vor Ankunft
- Sturmwarnung → alle Rollos runter
- Reinigungs-Status zwischen Buchungen
