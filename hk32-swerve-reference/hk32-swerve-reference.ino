#include <Arduino.h>

// HK32F103C8T6 Blue Pill style board, using STM32duino pin names.
// Assumes the HK32 behaves compatibly with the STM32F103C8 Arduino core.
// Both drive motors start running as soon as the sketch starts.

const uint32_t STEP1_PIN = PA0;
const uint32_t DIR1_PIN  = PA1;
const uint32_t STEP2_PIN = PA2;
const uint32_t DIR2_PIN  = PA3;

const uint32_t IN1_PIN = PB0;
const uint32_t IN2_PIN = PB1;
const uint32_t IN3_PIN = PB10;
const uint32_t IN4_PIN = PB11;

const uint32_t ENC1_PIN = PA6;
const uint32_t ENC2_PIN = PA7;

// FTDI: board PA9 (TX) -> FTDI RX, board PA10 (RX) <- FTDI TX.
HardwareSerial DebugPort(PA10, PA9);

volatile uint32_t enc1Ticks = 0;
volatile uint32_t enc2Ticks = 0;

void isrEnc1() { ++enc1Ticks; }
void isrEnc2() { ++enc2Ticks; }

void setup() {
  DebugPort.begin(115200);

  pinMode(STEP1_PIN, OUTPUT);
  pinMode(DIR1_PIN, OUTPUT);
  pinMode(STEP2_PIN, OUTPUT);
  pinMode(DIR2_PIN, OUTPUT);
  digitalWrite(STEP1_PIN, LOW);
  digitalWrite(STEP2_PIN, LOW);

  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);
  digitalWrite(IN1_PIN, LOW);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, LOW);
  digitalWrite(IN4_PIN, LOW);

  pinMode(ENC1_PIN, INPUT_PULLUP);
  pinMode(ENC2_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC1_PIN), isrEnc1, RISING);
  attachInterrupt(digitalPinToInterrupt(ENC2_PIN), isrEnc2, RISING);

  DebugPort.println("2 steppers + 2 DC motors + 2 encoders ready");
}

void loop() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);

  digitalWrite(DIR1_PIN, HIGH);
  digitalWrite(DIR2_PIN, HIGH);

  for (int i = 0; i < 200; ++i) {
    digitalWrite(STEP1_PIN, HIGH);
    digitalWrite(STEP2_PIN, HIGH);
    delayMicroseconds(1000);
    digitalWrite(STEP1_PIN, LOW);
    digitalWrite(STEP2_PIN, LOW);
    delayMicroseconds(1000);
  }

  noInterrupts();
  const uint32_t ticks1 = enc1Ticks;
  const uint32_t ticks2 = enc2Ticks;
  interrupts();

  DebugPort.print("Enc 1 Ticks: ");
  DebugPort.print(ticks1);
  DebugPort.print(" | Enc 2 Ticks: ");
  DebugPort.println(ticks2);
  delay(1000);
}
