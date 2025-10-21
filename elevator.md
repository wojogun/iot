# Elevator Pitch: Smart Beach Resort – IoT für intelligente Ferienhäuser

## [0:00 – 0:45] Problem und Motivation
Ferienhäuser an der Küste sind schön – aber technisch anspruchsvoll.
Salznebel, Feuchtigkeit, Sand und Wind belasten Gebäude und Geräte.
Betreiber müssen außerdem Reinigung, Energie, Gäste-Zugang und Sicherheit koordinieren – oft aus der Ferne.
Unser Ziel: ein System, das diese Herausforderungen automatisiert bewältigt.
Wir wollen zeigen, dass sich mit erschwinglicher IoT-Technik ein komplettes digitales Resort steuern lässt – einfach, sicher und nachhaltig.

## [0:45 – 1:45] Unsere Lösung
Unser Projekt heißt Smart Beach Resort.
Es besteht aus vier intelligenten Modellhäusern, die über Sensoren, Aktoren und Cloud-Kommunikation miteinander verbunden sind.
Jedes Haus hat eine eigene Aufgabe:

**Haus 1** Wetter- und Sturmüberwachung. Es misst Temperatur, Feuchtigkeit und Gaswerte und löst Sturmwarnungen aus.

**Haus 2** Gäste-Komfort und Sicherheit. RFID-Zugang, Service-Buttons, automatische Rollos und Lichtsteuerung.

**Haus 3** Energie und Wartung. Überwacht Lüftung, meldet Reinigungsstatus und Wartungsbedarf.

**Haus 4** Umwelt und Nachhaltigkeit. Erfasst Licht, Boden- und Luftfeuchte, aktiviert Eco-Modus und reduziert Ressourcenverbrauch.

Alle vier Häuser sind über das MQTT-Protokoll verbunden und kommunizieren über die Cloud.
Ein Node-RED-Dashboard dient als Zentrale: Hier sieht der Betreiber live Temperatur, Feuchtigkeit, Energie- und Umweltwerte – und kann eingreifen, wenn nötig.

## [1:45 – 3:00] Technologie und Architektur
Jedes Haus verwendet einen ESP32-Mikrocontroller – das Herzstück unserer IoT-Architektur.
Die Datenübertragung läuft über HiveMQ Cloud – ein sicheres, Cloud-fähiges MQTT-Framework.
Damit funktioniert die Kommunikation auch hinter mobilen Hotspots, ohne komplexe Netzwerkkonfiguration.
Node-RED auf unserem Server übernimmt die Logik:
Wenn Haus 1 einen Sturm erkennt, schickt Node-RED Befehle an alle anderen Häuser:
Haus 2 schließt Rollos, Haus 3 stoppt Lüftung, Haus 4 deaktiviert Bewässerung.
Nach dem Ereignis gehen alle Systeme automatisch in den Normalmodus zurück.
So entsteht ein echtes, verteiltes IoT-Ökosystem – nicht nur ein Smart Home, sondern ein Smart Resort.

## [3:00 – 4:00] Mehrwert und Vorteile
Der größte Vorteil: Automatisierung.
Der Betreiber sieht alle wichtigen Informationen zentral und muss nicht ständig vor Ort sein.
Das spart Energie, Personal und Reaktionszeit.
Außerdem denken wir Nachhaltigkeit mit:
Haus 4 reduziert Wasser- und Energieverbrauch, Haus 3 optimiert Lüftungszeiten, und Haus 1 schützt Gebäude vor Feuchtigkeitsschäden.
Die Kommunikation über HiveMQ Cloud ist skalierbar – das System könnte morgen nicht nur vier, sondern vierzig Häuser verwalten.
Und weil alles auf offenen Standards basiert (ESP32, MQTT, Node-RED), bleibt die Lösung erweiterbar und vendor-unabhängig.

## [4:00 – 4:45] Demonstration und Umsetzung
Für die Präsentation verwenden wir vier Keyestudio-Modellhäuser KS5009.
Jedes ist über ein Handy-Hotspot mit der Cloud verbunden.
Wir zeigen live, wie eine Sturmwarnung in Haus 1 alle anderen Häuser synchron steuert.
Alle Sensordaten erscheinen sofort im Dashboard.
Das ist ein realistisches Mini-Abbild eines vernetzten Ferienresorts – technisch robust, einfach verständlich und optisch überzeugend.

## [4:45 – 5:00] Fazit und Ausblick
Smart Beach Resort beweist, dass sich komplexe Gebäudesteuerung auch mit einfachen Mitteln realisieren lässt.
Die Architektur ist Cloud-fähig, modular und skalierbar – und kann künftig um Smart-City- oder Energie-Sharing-Funktionen erweitert werden.
Unser Ziel war, aus vier einzelnen Häusern ein intelligentes, kooperierendes System zu machen.
Ergebnis: Ein kleines Modell mit großem Potenzial – ein smarter Strand, der wirklich mitdenkt.
