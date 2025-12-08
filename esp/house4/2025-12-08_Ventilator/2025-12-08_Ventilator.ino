const int FAN_PIN = 19;  // Relais / Lüfter an GPIO19

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("🚀 Ventilator-Zyklus mit PIN 19 (3s)");

  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);  // AUS (ggf. invertiert testen)

  // 1. Lauf: 3 Sekunden AN
  Serial.println("🔁 Lauf 1: AN (3s)");
  digitalWrite(FAN_PIN, HIGH);   // ggf. LOW, falls Relais invertiert
  delay(3000);

  // Pause 1: 3 Sekunden AUS
  Serial.println("⏸ Pause 1: AUS (3s)");
  digitalWrite(FAN_PIN, LOW);    // ggf. HIGH
  delay(3000);

  // 2. Lauf: 3 Sekunden AN
  Serial.println("🔁 Lauf 2: AN (3s)");
  digitalWrite(FAN_PIN, HIGH);   // ggf. LOW
  delay(3000);

  // Ende: dauerhaft AUS
  Serial.println("⏹ Ende: AUS");
  digitalWrite(FAN_PIN, LOW);    // ggf. HIGH
}

void loop() {
  // nichts – Ablauf nur einmal
}
