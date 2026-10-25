#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);

#define OLED_ADDR 0x3C
Adafruit_SH1106G display(128, 64, &Wire, -1);

#define BUZZER 3
#define GREEN 6
#define RED 7
#define YELLOW 8
#define SWITCH 4

// Replace this placeholder with the UID of a non-sensitive test tag.
byte blueUID[4] = {0x00, 0x00, 0x00, 0x00};

void setup() {
  Serial.begin(9600);
  Wire.begin();

  display.begin(OLED_ADDR, true);
  display.clearDisplay();
  display.setTextColor(SH110X_WHITE);

  SPI.begin();
  mfrc522.PCD_Init();

  pinMode(BUZZER, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);
}

void loop() {

  if (digitalRead(SWITCH) == HIGH) {
    show("SYSTEM OFF", "");
    allOff();
    return;
  }

  digitalWrite(YELLOW, HIGH);
  show("Scan Card", "");

  if (!mfrc522.PICC_IsNewCardPresent()) return;
  if (!mfrc522.PICC_ReadCardSerial()) return;

  digitalWrite(YELLOW, LOW);

  // Debug UID print
  Serial.print("UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i], HEX);
    Serial.print(" ");
  }
  Serial.println();

  // 🔥 MATCH CHECK
  bool isBlue = true;

  for (byte i = 0; i < 4; i++) {
    if (mfrc522.uid.uidByte[i] != blueUID[i]) {
      isBlue = false;
    }
  }

  if (isBlue) {
    // ✅ BLUE TAG → ACCEPT
    digitalWrite(GREEN, HIGH);
    show("ACCESS", "GRANTED");

    digitalWrite(BUZZER, HIGH); delay(200);
    digitalWrite(BUZZER, LOW); delay(100);
    digitalWrite(BUZZER, HIGH); delay(200);
    digitalWrite(BUZZER, LOW);

    delay(2000);
    digitalWrite(GREEN, LOW);

  } else {
    // ❌ ANY OTHER CARD → DENY
    digitalWrite(RED, HIGH);
    show("ACCESS", "DENIED");

    digitalWrite(BUZZER, HIGH);
    delay(1000);
    digitalWrite(BUZZER, LOW);

    delay(2000);
    digitalWrite(RED, LOW);
  }

  mfrc522.PICC_HaltA();
}

// ===== FUNCTIONS =====

void show(String a, String b) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 5);
  display.println(a);
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.println(b);
  display.display();
}

void allOff() {
  digitalWrite(GREEN, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(BUZZER, LOW);
}