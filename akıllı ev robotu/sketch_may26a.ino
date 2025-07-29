#include <WiFi.h>
#include <HTTPClient.h>

// Wi-Fi bilgileri
const char* ssid = "";
const char* password = "";

// Flask sunucu adresi
const char* serverName = "http://192.168.64.189:5000/mesafe";

// Step motor pinleri
#define L_STEP 4
#define L_DIR  5
#define R_STEP 27
#define R_DIR  26

// Sensör pinleri
#define TRIG_BACK 22
#define ECHO_BACK 23
#define TRIG_RIGHT 18
#define ECHO_RIGHT 19
#define TRIG_LEFT 25
#define ECHO_LEFT 13
#define TRIG_FRONT 33
#define ECHO_FRONT 32

int step_delay = 4000;         // Daha uzun bekleme süresi (yavaş hareket)
int steps_per_move = 500;     // Daha kısa mesafe (daha az adım)

unsigned long lastSendTime = 0;
const unsigned long sendInterval = 500; // 0.5 saniyede bir veri gönder

float readDistanceCM(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  if (duration == 0) return -1;
  return duration * 0.034 / 2;
}

void moveMotors(bool leftDir, bool rightDir, int steps) {
  digitalWrite(L_DIR, leftDir);
  digitalWrite(R_DIR, rightDir);
  for (int i = 0; i < steps; i++) {
    digitalWrite(L_STEP, HIGH);
    digitalWrite(R_STEP, HIGH);
    delayMicroseconds(step_delay);
    digitalWrite(L_STEP, LOW);
    digitalWrite(R_STEP, LOW);
    delayMicroseconds(step_delay);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(L_STEP, OUTPUT); pinMode(L_DIR, OUTPUT);
  pinMode(R_STEP, OUTPUT); pinMode(R_DIR, OUTPUT);

  pinMode(TRIG_FRONT, OUTPUT); pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_BACK, OUTPUT);  pinMode(ECHO_BACK, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT); pinMode(ECHO_RIGHT, INPUT);
  pinMode(TRIG_LEFT, OUTPUT);  pinMode(ECHO_LEFT, INPUT);

  WiFi.begin(ssid, password);
  Serial.println("WiFi'ye bağlanılıyor...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Bağlanıyor...");
  }
  Serial.println("WiFi bağlantısı başarılı!");
  Serial.print("IP adresi: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  float distFront = readDistanceCM(TRIG_FRONT, ECHO_FRONT);
  float distBack  = readDistanceCM(TRIG_BACK, ECHO_BACK);
  float distRight = readDistanceCM(TRIG_RIGHT, ECHO_RIGHT);
  float distLeft  = readDistanceCM(TRIG_LEFT, ECHO_LEFT);

  Serial.print("ÖN: "); Serial.print(distFront);
  Serial.print("  ARKA: "); Serial.print(distBack);
  Serial.print("  SAĞ: "); Serial.print(distRight);
  Serial.print("  SOL: "); Serial.println(distLeft);

  bool on_acik   = distFront > 40 || distFront == -1;
  bool sag_acik  = distRight > 40 || distRight == -1;
  bool sol_acik  = distLeft  > 40 || distLeft  == -1;
  bool arka_acik = distBack  > 40 || distBack  == -1;

  if (on_acik) {
    Serial.println("İLERİ GİDİYOR...");
    moveMotors(HIGH, HIGH, steps_per_move);
  }
  else {
    if (sag_acik) {
      Serial.println("ENGEL VAR → SAĞA DÖNÜYOR");
      moveMotors(HIGH, LOW, steps_per_move / 2);
    }
    else if (sol_acik) {
      Serial.println("ENGEL VAR → SOLA DÖNÜYOR");
      moveMotors(LOW, HIGH, steps_per_move / 2);
    }
    else if (arka_acik) {
      Serial.println("ENGEL VAR → GERİ GİDİYOR");
      moveMotors(LOW, LOW, steps_per_move / 2);
    }
    else {
      Serial.println("⚠️ TÜM YÖNLER KAPALI → DURUYOR");
    }
  }

  if (millis() - lastSendTime >= sendInterval) {
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverName);
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");

      String postData = "front=" + String(distFront) +
                        "&back=" + String(distBack) +
                        "&right=" + String(distRight) +
                        "&left=" + String(distLeft);

      int httpResponseCode = http.POST(postData);
      if (httpResponseCode > 0) {
        String response = http.getString();
        Serial.println("Sunucudan yanıt: " + response);
      } else {
        Serial.print("HTTP Hatası: ");
        Serial.println(httpResponseCode);
      }

      http.end();
    }

    lastSendTime = millis();
  }
}