/*
  ESP32 Smart Home Automation with Blynk

  Replace the placeholders below with your credentials before uploading.
  Do NOT commit real credentials to public repositories.

  Required:
  - WiFi SSID & password
  - Blynk Template ID
  - Blynk Template Name
  - Blynk Auth Token
*/

// ================= USER CONFIG =================


#define BLYNK_TEMPLATE_ID   "YOUR_TEMPLATE_ID"
#define BLYNK_TEMPLATE_NAME "YOUR_TEMPLATE_NAME"
#define BLYNK_AUTH_TOKEN    "YOUR_BLYNK_AUTH_TOKEN"
// ==============================================
#define WIFI_SSID       "YOUR_WIFI_NAME"
#define WIFI_PASSWORD   "YOUR_WIFI_PASSWORD"
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include "DHT.h"

// --- WiFi Credentials ---
char ssid[] = WIFI_SSID;
char pass[] = WIFI_PASSWORD;

// --- DHT11 Setup ---
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// --- Pin Definitions ---
#define RELAY_LIGHT 26
#define RELAY_FAN   27
#define LDR_PIN     34
#define LED_PIN     14
#define TRIG_PIN    32
#define ECHO_PIN    33
#define LOCK_PIN    25
#define MQ135_PIN   35
#define BUZZER_PIN  2

// --- Thresholds ---
int lightThreshold = 3500;
const int DARKNESS_THRESHOLD = 3000;
const int MOTION_DISTANCE_CM = 100;
int GAS_THRESHOLD = 2600;

// --- Blynk Virtual Pins ---
#define VPIN_LIGHT V0
#define VPIN_FAN   V1
#define VPIN_TEMP  V2
#define VPIN_HUMID V3
#define VPIN_GAS   V4
#define VPIN_LOCK  V5
#define VPIN_LDR   V6
#define VPIN_GAS_THRESHOLD V7

BlynkTimer timer;
unsigned long lastNotification = 0;

// --- BLYNK Button Handlers ---
BLYNK_WRITE(VPIN_LIGHT) {
  digitalWrite(RELAY_LIGHT, param.asInt());
}

BLYNK_WRITE(VPIN_FAN) {
  digitalWrite(RELAY_FAN, param.asInt());
}

BLYNK_WRITE(VPIN_GAS_THRESHOLD) {
  GAS_THRESHOLD = param.asInt();
}

// --- DHT Data ---
void sendDHTData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(temp) && !isnan(hum)) {
    if (temp > -10 && temp < 60 && hum >= 0 && hum <= 100) {
      Blynk.virtualWrite(VPIN_TEMP, temp);
      Blynk.virtualWrite(VPIN_HUMID, hum);
      Serial.printf("Temperature: %.2f°C | Humidity: %.2f%%\n", temp, hum);
    }
  } else {
    Serial.println("Failed to read from DHT sensor!");
  }
}

// --- LDR Sensor ---
void checkLDR() {
  int ldrValue = analogRead(LDR_PIN);
  ldrValue = constrain(ldrValue, 0, 4000);
  Blynk.virtualWrite(VPIN_LDR, ldrValue);
  digitalWrite(LED_PIN, (ldrValue > lightThreshold) ? HIGH : LOW);
  Serial.printf("LDR Value: %d\n", ldrValue);
}

// --- Ultrasonic Sensor ---
void checkUltrasonic() {
  long distance = getUltrasonicDistance();
  if (distance > 0 && distance < MOTION_DISTANCE_CM) {
    digitalWrite(LOCK_PIN, HIGH);
    Blynk.virtualWrite(VPIN_LOCK, 1);
    Serial.println("🔒 Motion Detected → Solenoid Lock Engaged");
  } else {
    digitalWrite(LOCK_PIN, LOW);
    Blynk.virtualWrite(VPIN_LOCK, 0);
    Serial.println("🔓 No Motion → Solenoid Lock Released");
  }
}

// --- Gas Sensor Handler ---
void checkGas() {
  int gasValue = analogRead(MQ135_PIN);
  Blynk.virtualWrite(VPIN_GAS, gasValue);
  Serial.printf("MQ135 Gas Value: %d | Threshold: %d\n", gasValue, GAS_THRESHOLD);

  if (gasValue > GAS_THRESHOLD) {
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("🚨 Gas Detected → Buzzer ON");

    if (millis() - lastNotification > 60000) {
      Blynk.logEvent("gas_alert", "🚨 High gas level detected!");
      lastNotification = millis();
    }
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("✅ Air Normal → Buzzer OFF");
  }
}

// --- Ultrasonic Distance ---
long getUltrasonicDistance() {
  long sum = 0;
  int valid = 0;
  for (int i = 0; i < 3; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    if (duration > 0) {
      sum += duration * 0.034 / 2;
      valid++;
    }
    delay(50);
  }
  return (valid > 0) ? (sum / valid) : -1;
}

// --- Setup ---
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_LIGHT, OUTPUT);
  pinMode(RELAY_FAN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LOCK_PIN, OUTPUT);
  pinMode(MQ135_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_LIGHT, LOW);
  digitalWrite(RELAY_FAN, LOW);
  digitalWrite(LOCK_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  timer.setInterval(2000L, sendDHTData);
  timer.setInterval(1000L, checkLDR);
  timer.setInterval(2000L, checkUltrasonic);
  timer.setInterval(2000L, checkGas);
}

// --- Loop ---
void loop() {
  Blynk.run();
  timer.run();
}
