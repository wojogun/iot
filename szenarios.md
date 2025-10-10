# Projekt-Szenario: Smart Beach Resort am niederländischen Strand

Unser Projekt umfasst drei intelligente Ferienhäuser, die in unmittelbarer Nähe zum niederländischen Meeresstrand stehen und über Airbnb vermietet werden. Die besonderen Herausforderungen dieser Lage sind die hohe Luftfeuchtigkeit, Sandstürme, Salzwasser-Korrosion und die Notwendigkeit einer effizienten Ferienhaus-Verwaltung.

## Individuelle Use Cases

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

## Gemeinsamer Use Case: Smart Beach Resort Management

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

# Szenario der Gruppe B
Wir haben drei mittels Online-Buchungssystem mietbare Häuser an der niederländischen Nordseeküste. Die Usecases sind dabei folgende:
## Haus 1: Wetter & Sturm-Überwachung
- Temp and Hum Sensor für Innenklima
- Steam Sensor für Feuchtigkeit (Salzwasser-Korrosion)
- PIR Motion Sensor für unerlaubte Zutritte
- Gas Sensor für Heizung/Gas-Überwachung
- Passive Buzzer für Sturmwarnungen
## Haus 2: Gäste-Komfort & Sicherheit
- RFID Module für Schlüssel-Ersatz (Gäste-Cards)
- Button Module für Service-Anfragen
- LCD Display für Check-in/Check-out Info
- RGB LED für Stimmungs-Beleuchtung
- Servo Motor für automatische Rollos (Sandschutz)
## Haus 3: Energie & Wartung
- Motor/Fan für Lüftung (Sand-Filter)
- Passive Buzzer für Wartungs-Alerts
- Button Module für Reinigungs-Status
- LCD Display für Energieverbrauch
- Gas Sensor für CO-Monitoring

## Gemeinsamer Use Case: Smart Beach Resort Management
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

# Umsetzung

## IoT-Framework: HiveMQ + Node-RED
### Überblick
Für das Projekt *Smart Beach Resort* wird eine **Kombination aus HiveMQ Cloud** (MQTT-Framework) und **Node-RED** (Orchestrierungs- und Visualisierungsplattform) gewählt.  
Diese Architektur erfüllt alle technischen und organisatorischen Anforderungen.

### 1. Framework-Komponenten
| Ebene | Technologie | Funktion |
|-------|--------------|-----------|
| Geräteebene | **ESP32 + Sensorik** | Lokale Datenerfassung, MQTT-Client |
| Kommunikations-Framework | **HiveMQ Cloud** | Sichere, skalierbare IoT-Kommunikation über MQTT |
| Steuerungs-/Integrations-Ebene | **Node-RED** | Datenfluss-Logik, zentrale Steuerung, Visualisierung |
| Benutzer-Ebene | **Web-Dashboard / Mobile App** | Zugriff für Host und Gäste |

### 2. Begründung der Framework-Wahl
**systemisch** adressiert die Kombination von HiveMQ und Node-RED alle Schichten einer IoT-Architektur:

| Schicht | Framework | Hauptfunktion |
|----------|------------|----------------|
| IoT-Gerät | ESP32 | Sensordatenerfassung und MQTT-Client |
| Kommunikation | HiveMQ | Sichere, skalierbare Datenübertragung |
| Logik / Steuerung | Node-RED | Prozesslogik, Datenrouting, Reaktionen |
| Visualisierung | Node-RED Dashboard | Monitoring, Steuerung, Status |
| Integration | HiveMQ / Node-RED APIs | Anbindung an Cloud, Datenbanken oder Mobile Apps |

**Kommunikation**
HiveMQ erfüllt zentrale Anforderungen eines modernen IoT-Frameworks auf der **Kommunikationsebene**:

| Kriterium | Beschreibung |
|------------|--------------|
| **Standardisierung** | Unterstützt MQTT 3.1.1 und 5.0 – das weltweit etablierte IoT-Kommunikationsprotokoll. |
| **Interoperabilität** | Funktioniert mit allen gängigen Mikrocontrollern (ESP32, Raspberry Pi, Arduino). Keine herstellerspezifischen Bindungen. |
| **Sicherheit** | TLS-Verschlüsselung, Benutzer-Authentifizierung, Zugriffsbeschränkungen (ACLs). |
| **Skalierbarkeit** | Cloud-nativ, skalierbar auf Hunderte Clients. Ideal auch für spätere Erweiterung (weitere Häuser, Sensoren). |
| **Zuverlässigkeit** | Session-Persistence, Last Will, QoS-Level (0–2) für gesicherte Nachrichtenübertragung. |
| **Cloud-Fähigkeit** | Vollständig gemanagte Umgebung, keine lokale Installation nötig. Zugriff auch über Hotspot möglich. |
| **On-Premise-Fähigkeit** | Mit HiveMQ Enterprise oder lokalem Docker-Setup möglich – OnPrem als Alternative beschreibbar. |
| **Integrationen** | REST-API, Kafka-Connector, WebSocket-API, Monitoring-Tools → Integration mit Data Analytics oder externem Dashboard. |

→ **Fazit:** HiveMQ ist nicht nur ein MQTT-Broker, sondern ein **IoT-Kommunikations-Framework**, das zentrale Aufgaben von Datenerfassung, Sicherheitsmanagement und Integrationslogik übernimmt.

---

**Steuerung**
Node-RED erweitert HiveMQ auf der **Prozess- und Integrations-Ebene**:

| Kriterium | Beschreibung |
|------------|--------------|
| **Flow-basierte Entwicklung** | Visuelle Programmierung ermöglicht schnelle Logik-Erstellung ohne komplexen Code. |
| **MQTT-native Integration** | Direkte Unterstützung für HiveMQ-Themen (Publish/Subscribe) – keine zusätzliche Middleware nötig. |
| **Echtzeitverarbeitung** | Ereignisgesteuerte Reaktionen (z. B. Sturmwarnung → Fenster schließen in allen Häusern). |
| **Web-Dashboard** | Eingebaute Visualisierung für Sensorwerte, Warnungen und Gerätestatus. |
| **Erweiterbarkeit** | Hunderte Nodes für API-Aufrufe, Datenbanken, Logging und Cloud-Dienste. |
| **Cloud- und Serverfähigkeit** | Betrieb auf eigenem Server (Docker, Linux, Windows) oder auf Plattformen wie Render/Railway. |
| **On-Premise-Betrieb** | Node-RED kann lokal mit Mosquitto oder HiveMQ Enterprise genutzt werden. |

→ **Fazit:** Node-RED bildet das **logische Framework** zur Steuerung, Analyse und Visualisierung des Smart-Home-Systems.


### 3. Vorteile gegenüber Alternativen
| Framework | Bewertung | Nachteile im Projektkontext |
|------------|------------|-----------------------------|
| **AWS IoT Core** | sehr leistungsfähig | Komplexe Einrichtung, kostenpflichtig, zu schwergewichtig |
| **Azure IoT Hub** | skalierbar | Hoher Setup-Aufwand, proprietär, Cloud-only |
| **Home Assistant / ESPHome** | einfach im LAN | Nicht Cloud-fähig, erfordert lokale Infrastruktur |
| **Adafruit IO / Blynk** | schnell startklar | Begrenzte Flexibilität, API-Einschränkungen, proprietär |
| **Mosquitto (lokal)** | leichtgewichtig | Keine Cloud-Reichweite, NAT-Probleme bei Hotspots |
| **HiveMQ + Node-RED** | optimal | Stabil, Cloud-fähig, modular, sicher, kostenlos im Free-Tier |

→ **Schlussfolgerung:** HiveMQ + Node-RED bietet das beste Verhältnis aus Einfachheit, Stabilität und Cloud-Tauglichkeit für ein verteiltes IoT-Szenario mit Hotspot-Verbindungen.

### 4. Cloud- und On-Premise-Fähigkeit
| Einsatzform | Beschreibung |
|--------------|--------------|
| **Cloud** | HiveMQ Cloud + Node-RED (auf eigenem oder Cloud-Server)|
| **On-Premise** | HiveMQ Enterprise + Node-RED lokal (zB. Raspberry Pi oder PC) möglich, aber nur bei stabiler LAN/WLAN-Infrastruktur praktikabel |

### 5. Kostenübersicht
| Komponente | Kosten |
|-------------|--------|
| HiveMQ Cloud (Serverless) | kostenlos (Free Tier) |
| Node-RED (eigener Server) | kostenlos, nur Stromkosten |
| Alternativ: Node-RED auf vorhandenem Server |
| Hardware (3 × KS5009-Kits) | von FH bereitgestellt |
| **Gesamt** | **ß € einmalig + 0 €/Monat** |

### 6. Fazit
HiveMQ fungiert als **IoT-Kommunikationsframework** mit klarer Geräte-, Sicherheits- und Integrationslogik.  
Node-RED erweitert es zu einem vollständigen **End-to-End-IoT-System**, das Cloud-fähig, verteilbar und präsentationssicher ist.  
Diese Kombination ist robust, leicht wartbar und erfüllt die Anforderungen an den Gruppen- und Gesamt-Use-Case.

## sonstige Fragen
### Kann man HiveMQ überhaupt als IoT-Framework verstehen? Warum?
Ja. HiveMQ erfüllt die Kriterien eines IoT-Frameworks.

**Begründung:**
1. per **Definition**
   Ein IoT-Framework ist eine Infrastruktur, die das **Sammeln, Übertragen, Verarbeiten und Visualisieren** von Sensordaten zwischen verteilten Geräten, Gateways und Cloud-Diensten ermöglicht.

2. per **Funktionalität**
   HiveMQ bietet genau diese Infrastruktur:
- **Nachrichtenvermittlung (MQTT Broker):** verbindet viele Geräte gleichzeitig, bidirektional.
- **Sicherheits-Layer:** TLS, Auth/ACLs, Session-Management.
- **Integrationen:** REST-API, WebSocket-API, Kafka-Connector, Prometheus-Monitoring, Cloud-Connectoren.
- **Skalierbarkeit:** Cluster-fähig (HiveMQ Enterprise/Cloud), Auto-Scaling in Cloud-Umgebungen.
- **Erweiterbarkeit:** durch MQTT-Extensions, etwa Daten-Filter, Rule-Engines, Persistenz.
  → Damit bildet HiveMQ die *Middleware-Schicht* zwischen Hardware (ESP32-Sensorik) und Applikation (Node-RED, Dashboard, App).

3. **Warum als Framework anerkennbar**
   Ein Framework muss nicht zwingend „vollständige Applikationslogik“ enthalten. In der IoT-Systemarchitektur ist das Messaging-Framework (MQTT-Broker) das Kernstück der Kommunikation und Integrationslogik. HiveMQ stellt also das **IoT-Kommunikationsframework** bereit, auf dem unsere Anwendung aufbaut.

4. **Cloud-Fähigkeit**
   HiveMQ Cloud ist vollständig Cloud-basiert (Serverless oder Managed Cluster). Geräte verbinden sich per TLS aus beliebigen Netzen (Handy-Hotspot, LAN, Campus). → Hochverfügbarkeit ohne eigenen Server.

**Zusammengefasst**
   HiveMQ = IoT-Framework auf der Kommunikations- und Integrations-Ebene:
- Gerätekommunikation (MQTT)
- Security & Session Management
- Skalierung
- Integration in Analytik- und Managementsysteme

Wir ergänzen dieses Framework durch Node-RED (Prozess-/Automationslogik) und die ESP32-Firmware (Geräteebene).
→ Das ergibt eine vollständige IoT-Systemarchitektur.
