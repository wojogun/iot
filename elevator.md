**Elevator Pitch – Smart Beach Resort**
*(Dauer: ca. 5 Minuten, Sprechgeschwindigkeit 120 Wörter/Minute ≈ 600 Wörter)*
In Form von: Mitarbeiter redet Chef im Aufzug an...
Überschriften dienen nur der Strukturierung - nicht vortragen!
---

### **Einleitung – Interesse wecken (0:00 – 1:00)**

Stellen Sie sich ein Ferienresort vor, das sich selbst organisiert – vom Check-in über die Raumluft bis hin zur Sturmwarnung.
Genau das demonstriert unser Projekt **Smart Beach Resort**: ein vollständig vernetztes IoT-System, das vier Ferienhäuser zu einem intelligenten Gesamtsystem verbindet.
Ziel ist es, ein **funktionsfähiges, modulares und cloudbasiertes IoT-Ökosystem** aufzubauen, das zentrale Aufgaben wie Gäste-Management, Umweltüberwachung, Sicherheit und Entertainment digital abbildet.
Das Projekt zeigt nicht Theorie, sondern Umsetzung – von der Sensordatenerfassung über MQTT-Kommunikation bis hin zur Visualisierung im zentralen Dashboard mit **Node-RED**.

---

### **Systemübersicht (1:00 – 2:00)**

Alle vier Häuser basieren auf dem **ESP32-Controller** aus dem Keyestudio-Kit KS5009.
Sie sind über das Internet mit der **HiveMQ Cloud** verbunden, die den Datenaustausch über das IoT-Protokoll **MQTT** ermöglicht.
**Node-RED**, betrieben in einem Docker-Container, übernimmt die Steuerung und Visualisierung.
Es sammelt Sensordaten, wertet sie aus, reagiert auf Ereignisse und stellt sie im Dashboard bereit – für Host, Servicepersonal und Gäste.
Die Architektur deckt damit alle Ebenen eines realen IoT-Systems ab:
**Geräte → Kommunikation → Logik → Visualisierung → Benutzer.**
Das Resort fungiert somit als Modell für eine skalierbare, cloudbasierte Infrastruktur, wie sie in modernen Ferienanlagen oder Smart-Building-Umgebungen eingesetzt werden kann.

---

### **Einzelne Häuser (2:00 – 4:00)**

**Haus 1 – Wetter & Perimeterschutz**
Haus 1 dient als zentrales Warnsystem. Es misst Temperatur, Luftfeuchtigkeit und Bewegung.
Bei hoher Feuchte oder einer simulierten Sturmwarnung aktiviert es den *Sturmmodus* – alle Häuser schließen Fenster, stoppen Lüftung und schalten Warnsignale.
Das LCD-Display zeigt aktuelle Wetterdaten, der Buzzer gibt akustische Hinweise.

**Haus 2 – Gäste-Komfort & Sicherheit**
Hier liegt der Fokus auf dem Aufenthalt. Ein **RFID-Sensor** ersetzt den Schlüssel, Buttons ermöglichen Check-out oder Serviceanfragen.
Ein **Servo** simuliert Rollos oder Türen, während eine **RGB-LED** den Belegungsstatus signalisiert.
Das System bildet so einen automatisierten, kontaktlosen Gästeprozess ab – technisch plausibel und übertragbar auf reale Vermietungssysteme.

**Haus 3 – Partyhaus**
Dieses Haus steht für Entertainment und Szenensteuerung.
Über das Dashboard kann eine *Party* aktiviert werden – **RGB-LEDs** und **Buzzer** erzeugen Licht- und Ton-Effekte.
Wird der *Sturmmodus* ausgelöst, beendet das System automatisch alle Aktivitäten – eine klare Demonstration zentraler Steuerlogik über mehrere IoT-Knoten hinweg.

**Haus 4 – Luftqualität & Automatische Lüftung**
Haus 4 sorgt für Sicherheit und Komfort. Ein **Gas-Sensor** überwacht CO- oder H₂-Konzentrationen.
Bei Überschreitung startet automatisch der **Ventilator**, das Dashboard zeigt eine Warnung, und ein Signalton informiert den Betreiber.
Sinken die Werte wieder, schaltet das System selbstständig zurück in den Normalbetrieb.
Ein **Button** dient zur manuellen Rücksetzung.

---

### **Gesamtsystem und Kommunikation (4:00 – 4:30)**

Alle vier Häuser sind über MQTT-Topics in der **HiveMQ Cloud** vernetzt.
**Node-RED** fungiert als zentrale Steuer- und Analyseinstanz.
Es synchronisiert gemeinsame Ereignisse wie den *Sturmmodus*, visualisiert Sensorwerte, dokumentiert Systemzustände und ermöglicht gezielte Eingriffe.
So entsteht ein vollständig integriertes IoT-Ökosystem – modular, erweiterbar und betriebssicher.

---

### **Schluss – Warum es finanzierbar ist (4:30 – 5:00)**

Das *Smart Beach Resort* zeigt, wie sich komplexe IoT-Konzepte mit günstiger, verfügbarer Hardware praktisch realisieren lassen.
Das System ist offen, skalierbar und direkt auf andere Anwendungen übertragbar – etwa Energieoptimierung, Smart-Office-Umgebungen oder Hotel-Automation.
Mit geringem Investitionsaufwand entsteht eine **reale Demonstrationsplattform**, die Technik-, Informatik- und Wirtschaftsaspekte vereint.
Eine Finanzierung dieses Projekts bedeutet, in eine **funktionsfähige IoT-Lehr- und Testumgebung** zu investieren – eine Basis, um Studierende, Entwickler oder Entscheidungsträger praxisnah an moderne IoT-Systemarchitekturen heranzuführen.

---


