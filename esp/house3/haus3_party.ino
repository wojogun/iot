/************************************************************
 * Haus 3 – Partyhaus (ESP32, HTTP + MQTT)
 ************************************************************/

// ======================== Includes ========================
#include "config.h"
#include "html.h"
String lastHttpMessage = "";

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <time.h>

#include <PubSubClient.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <BuzzerESP32.h>

// HTTP-Server
WiFiServer httpServer(80);

// MQTT-Topics
const char* TOPIC_CMD_PARTY     = "resort/house3/party/cmd";
const char* TOPIC_CMD_STORM     = "resort/house3/storm/cmd";
const char* TOPIC_BC_PARTY      = "resort/broadcast/party";
const char* TOPIC_BC_STORM      = "resort/broadcast/storm";
const char* TOPIC_STATUS_HOUSE3 = "resort/house3/status";
const char* TOPIC_NEXT_PARTY    = "resort/house3/party/next";

// MQTT-Client
WiFiClientSecure secureClient;
PubSubClient     mqttClient(secureClient);

// ======================== Hardware-Pins ========================
#define PIN_LED_YELLOW   12 // gelbe Warn-LED (Sturmmodus)
#define PIN_NEOPIXEL     26 // NeoPixel-Datenpin (SK6812)
#define NEOPIXEL_COUNT    4 // Anzahl NeoPixel
#define PIN_SERVO_DOOR   13 // Tür-Servo
#define PIN_SERVO_WINDOW  5 // Fenster-Servo
#define PIN_BUZZER       25 // Buzzer

// Servo-Winkel
const int DOOR_CLOSED   = 0;
const int DOOR_OPEN     = 180;
const bool USE_DOOR     = true;

const int WINDOW_CLOSED = 0;    // wie in pj5_2: myservo.write(0);
const int WINDOW_OPEN   = 176;
const bool USE_WINDOW   = false;

String nextPartyText = "keine Party"; 

// ======================== Objekte ========================
LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_NeoPixel strip(NEOPIXEL_COUNT, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
Servo doorServo;
Servo windowServo;
BuzzerESP32  buzzer(PIN_BUZZER);

// ======================== Zustände ========================
enum Mode {
  MODE_NORMAL,
  MODE_PARTY,
  MODE_STORM
};

Mode currentMode = MODE_NORMAL;

// Effekt-Zustände
unsigned long lastPartyUpdate = 0;
uint16_t      partyHue        = 0;

unsigned long lastStormBlink  = 0;
bool          stormLedState   = false;

void ctrDoor(unsigned int status) {
  if (USE_DOOR) {
    doorServo.write(status);
  } else {
    Serial.println("Door control is disabled due to servo-probs");
  }
}
void ctrWindow(unsigned int status) {
  if (USE_WINDOW) {
    windowServo.write(status);
  } else {
    Serial.println("Window control is disabled due to servo-probs");
  }
}

// ======================== Funktionsprototypen ========================
void initSerial();
void initLCD();
void initPins();
void initServoDoor();
void initNeoPixel();
void initBuzzer();
void initWiFi();
void initMDNS();
void initHTTPServer();
void initMQTT();
void showStartupInfo();

// Hauptlogik
void maintainMQTT();
void handleHttpClient();
void updateEffects();

// Effekt-Schritte
void partyLightsStep(unsigned long now);
void stormBlinkStep(unsigned long now);

// Zustandswechsel
void startParty(bool publish = true);
void stopParty(bool publish = true);
void startStorm(bool publish = true);
void stopStorm(bool publish = true);

// Musik
void smokeOnTheWater();
void werHatAnDerUhrGedreht();

// MQTT
void mqttCallback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT();

// HTTP-Helfer
const long  gmtOffset_sec     = 0;          // UTC
const int   daylightOffset_sec = 3600;      // +1h, wenn Sommerzeit, sonst 0

// Zeit
void initTime() {
  Serial.println("Sync time via NTP...");
  configTime(gmtOffset_sec, daylightOffset_sec, "pool.ntp.org", "time.nist.gov");

  struct tm timeinfo;
  int retries = 0;
  while (!getLocalTime(&timeinfo) && retries < 50) {
    Serial.print(".");
    delay(200);
    retries++;
  }
  Serial.println();

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
  } else {
    Serial.print("Time: ");
    Serial.println(&timeinfo, "%Y-%m-%d %H:%M:%S");
  }
}

// ========================= SETUP & LOOP =================
void setup() {
  initSerial();
  initLCD();
  initWiFi();
  initTime();
  initMDNS();
  initHTTPServer();
  initMQTT();
  
  initPins();
  if (USE_DOOR) initServoDoor();
  if (USE_WINDOW) initServoWindow();
  initNeoPixel();
  initBuzzer();
  showStartupInfo();
}
 
void loop() {
  maintainMQTT();
  updateEffects();
  handleHttpClient();
}


// ================== INITIALISIERUNG =====================
void initSerial() {
  Serial.begin(115200);
  delay(100);
}

void initMDNS() {
  if (!MDNS.begin("haus3party")) {  
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
    MDNS.addService("http", "tcp", 80); 
  }
}

void initLCD() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Haus 3: Party");
  lcd.setCursor(0, 1);
  lcd.print("Init...");
}

void initPins() {
  pinMode(PIN_LED_YELLOW, OUTPUT);
  digitalWrite(PIN_LED_YELLOW, LOW);
}

void initServoDoor() {
  if (!USE_DOOR) return;
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  doorServo.setPeriodHertz(50);
  doorServo.attach(PIN_SERVO_DOOR, 1000, 2000);
  doorServo.write(WINDOW_OPEN);
}

void initServoWindow() {
  if (!USE_WINDOW) return;
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  windowServo.setPeriodHertz(50);
  windowServo.attach(PIN_SERVO_WINDOW, 1000, 2000);
  windowServo.write(WINDOW_OPEN);
}

void initNeoPixel() {
  strip.begin();
  strip.clear();
  strip.show();
}

void initBuzzer() {
  buzzer.setTimbre(5);
  buzzer.playTone(0, 0);
}

void initWiFi() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi verbinden");
  lcd.setCursor(0, 1);
  lcd.print("...");

  WiFi.mode(WIFI_STA);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  uint8_t tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 40) { // ~20 s Timeout
    delay(500);
    Serial.print(".");
    tries++;
  }
  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected, IP: ");
    Serial.println(WiFi.localIP());

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("IP:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP().toString());
  } else {
    Serial.println("WiFi FAILED");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi FAIL");
    lcd.setCursor(0, 1);
    lcd.print("check SSID/PW");
  }
}

void initHTTPServer() {
  httpServer.begin();
  Serial.println("HTTP server started");
}


void showStartupInfo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IP:");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP().toString());
}

String getModeText() {
  switch (currentMode) {
    case MODE_NORMAL: return "NORMAL";
    case MODE_PARTY:  return "PARTY";
    case MODE_STORM:  return "STURM";
    default:          return "UNBEKANNT";
  }
}

// ==================== MQTT ==============================
void initMQTT() {
  //secureClient.setCACert(ROOT_CA);
  secureClient.setInsecure();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);
  //reconnectMQTT();
}

void maintainMQTT() {
  static unsigned long lastAttempt = 0;
  unsigned long now = millis();

  if (mqttClient.connected()) {
    mqttClient.loop();
  } else {
    // alle 5 Sekunden ein Versuch
    if (now - lastAttempt > 5000) {
      lastAttempt = now;
      reconnectMQTT();
    }
  }
}

void reconnectMQTT() {
  Serial.print("Attempting MQTT connection... ");

  String clientId = "Haus3Party-";
  clientId += String(random(0xffff), HEX);

  // secureClient.setCACert(ROOT_CA); // reicht in initMQTT()

  if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASSWORD)) {
    Serial.println("connected");

    mqttClient.subscribe(TOPIC_CMD_PARTY);
    mqttClient.subscribe(TOPIC_CMD_STORM);
    mqttClient.subscribe(TOPIC_BC_STORM);
    mqttClient.subscribe(TOPIC_NEXT_PARTY); 
    
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL");
  } else {
    Serial.print("failed, rc=");
    Serial.print(mqttClient.state());
    Serial.println(" will retry later");
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  String t = String(topic);

  Serial.print("MQTT in [");
  Serial.print(t);
  Serial.print("]: ");
  Serial.println(msg);

  if (t == TOPIC_CMD_PARTY) {
    if (msg == "START") {
      startParty(false);
    } else if (msg == "STOP") {
      stopParty(false);
    }
  } else if (t == TOPIC_CMD_STORM || t == TOPIC_BC_STORM) {
    if (msg == "ON") {
      startStorm(false);
    } else if (msg == "OFF") {
      stopStorm(false);
    }
  } else if (t == TOPIC_NEXT_PARTY) {
    nextPartyText = msg;   // String aus der MQTT-Nachricht
    // Nur in NORMAL-Modus sofort anzeigen, bei Party/Storm bleibt Text
    if (currentMode == MODE_NORMAL) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Haus 3 bereit");
      lcd.setCursor(0, 1);
      lcd.print("Next: ");
      lcd.print(nextPartyText);  // max. 10–11 Zeichen sinnvoll
    }
  }
}

// ================== HTTP-VERARBEITUNG ===================
void handleHttpClient() {
  WiFiClient client = httpServer.available();
  if (!client) {
    return;
  }

  Serial.println("HTTP client connected");

  // Warten, bis Daten da sind (max. 5s)
  uint32_t start = millis();
  while (!client.available() && millis() - start < 5000) {
    delay(1);
  }
  if (!client.available()) {
    Serial.println("No HTTP data, closing client");
    client.stop();
    return;
  }

  String reqLine = client.readStringUntil('\r');
  client.readStringUntil('\n'); // Rest der Zeile verwerfen
  Serial.print("RAW request line: ");
  Serial.println(reqLine);

  int sp1 = reqLine.indexOf(' ');
  int sp2 = reqLine.indexOf(' ', sp1 + 1);
  String path = "/";
  if (sp1 != -1 && sp2 != -1) {
    path = reqLine.substring(sp1 + 1, sp2);
  }

  Serial.print("Parsed path: ");
  Serial.println(path);

  // Root-Seite (HTML mit Buttons)
  if (path == "/") {
    String ipStr = WiFi.localIP().toString();
    client.print(buildRootPage(ipStr, getModeText(), lastHttpMessage));
    return;
  }

  // Party steuern
  if (path == "/party/start") {
    startParty(true);
    lastHttpMessage = "Party gestartet";
    String ipStr = WiFi.localIP().toString();
    client.print(buildRootPage(ipStr, getModeText(), lastHttpMessage));
    client.stop();
    return;
  }

  if (path == "/party/stop") {
    stopParty(true);
    lastHttpMessage = "Party gestoppt";
    String ipStr = WiFi.localIP().toString();
    client.print(buildRootPage(ipStr, getModeText(), lastHttpMessage));
    client.stop();
    return;
  }

  if (path == "/storm/on") {
    startStorm(true);
    lastHttpMessage = "Sturmmodus EIN";
    String ipStr = WiFi.localIP().toString();
    client.print(buildRootPage(ipStr, getModeText(), lastHttpMessage));
    client.stop();
    return;
  }

  if (path == "/storm/off") {
    stopStorm(true);
    lastHttpMessage = "Sturmmodus AUS";
    String ipStr = WiFi.localIP().toString();
    client.print(buildRootPage(ipStr, getModeText(), lastHttpMessage));
    client.stop();
    return;
  }

  // Default: 404
  String ipStr = WiFi.localIP().toString();
  client.print(buildRootPage(ipStr, getModeText(), lastHttpMessage));
  client.stop();
  Serial.println("HTTP 404 sent");
}

// ==================== EFFEKTE / MODI ====================
void updateEffects() {
  unsigned long now = millis();

  if (currentMode == MODE_PARTY) {
    partyLightsStep(now);
  } else if (currentMode == MODE_STORM) {
    stormBlinkStep(now);
  }
}

void partyLightsStep(unsigned long now) {
  const unsigned long interval = 30;
  if (now - lastPartyUpdate < interval) return;
  lastPartyUpdate = now;

  for (int i = 0; i < NEOPIXEL_COUNT; i++) {
    int pixelHue = partyHue + (i * 65536L / NEOPIXEL_COUNT);
    uint32_t color = strip.gamma32(strip.ColorHSV(pixelHue));
    strip.setPixelColor(i, color);
  }
  strip.show();
  partyHue += 256;
}

void stormBlinkStep(unsigned long now) {
  const unsigned long interval = 500;
  if (now - lastStormBlink < interval) return;
  lastStormBlink = now;

  stormLedState = !stormLedState;
  digitalWrite(PIN_LED_YELLOW, stormLedState ? HIGH : LOW);
}

// ======================== Zustandswechsel ========================
void startParty(bool publish) {
  if (currentMode == MODE_STORM) {
    return;  // bei Sturm keine Party
  }

  currentMode = MODE_PARTY;

  windowServo.write(WINDOW_CLOSED);
  doorServo.write(DOOR_CLOSED);
  digitalWrite(PIN_LED_YELLOW, LOW);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Party laeuft");
  lcd.setCursor(0, 1);
  lcd.print("Haus 3");

  strip.show();
  smokeOnTheWater();

  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_PARTY, "STARTED");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "PARTY");
  }
}

void stopParty(bool publish) {
  if (currentMode != MODE_PARTY) {
    return;
  }
  werHatAnDerUhrGedreht();
  currentMode = MODE_NORMAL;

  strip.clear();
  strip.show();

  buzzer.playTone(0, 0);
  ctrWindow(WINDOW_OPEN);
  ctrDoor(DOOR_OPEN);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Party beendet");
  lcd.setCursor(0, 1);
  lcd.print("Next:");
  lcd.print(nextPartyText);

  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_PARTY, "STOPPED");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL");
  }
}

void startStorm(bool publish) {
  currentMode = MODE_STORM;

  windowServo.write(WINDOW_CLOSED);
  doorServo.write(DOOR_CLOSED);
  strip.clear();
  strip.show();
  buzzer.playTone(0, 0);

  digitalWrite(PIN_LED_YELLOW, HIGH);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("STURM!");
  lcd.setCursor(0, 1);
  lcd.print("Party STOP");

  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_STORM, "ON");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "STORM");
  }
}

void stopStorm(bool publish) {
  currentMode = MODE_NORMAL;

  digitalWrite(PIN_LED_YELLOW, LOW);
  ctrWindow(WINDOW_OPEN);
  ctrDoor(DOOR_OPEN);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Sturm vorbei");
  lcd.setCursor(0, 1);
  lcd.print("Next:");
  lcd.print(nextPartyText);

  if (publish && mqttClient.connected()) {
    mqttClient.publish(TOPIC_BC_STORM, "OFF");
    mqttClient.publish(TOPIC_STATUS_HOUSE3, "NORMAL");
  }
}

// ======================== Melodie ========================

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
  buzzer.playTone(440, E);   // A4 
  buzzer.playTone(392, Q);   // G4 
  buzzer.playTone(370, Q);   // F#4 

  buzzer.playTone(330, Q);   // E4 
  buzzer.playTone(494, Q);   // H4 
  buzzer.playTone(494, Q);   // H4 
  delay(Q);

  buzzer.playTone(554, Q);   // C#5
  buzzer.playTone(494, Q);   // H4 
  buzzer.playTone(440, Q);   // A4 
  buzzer.playTone(392, Q);   // G4 

  buzzer.playTone(370, Q);   // F#4
  buzzer.playTone(494, Q);   // H4  
  buzzer.playTone(440, Q);   // A4 
  delay(Q);
}
