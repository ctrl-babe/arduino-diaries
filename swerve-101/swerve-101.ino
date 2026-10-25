// Arduino/Genuino 101: two steering steppers, two DC motors, two encoders.
// Motor outputs turn on as soon as this sketch starts.
const int STEP_PIN = 4;
const int DIR_PIN = 7;
const int STEP2_PIN = 8;
const int DIR2_PIN = 12;

const int IN1_PIN = 5;
const int IN2_PIN = 6;
const int ENC1_PIN = 2;
const int IN3_PIN = A2;
const int IN4_PIN = A3;
const int ENC2_PIN = 3;

volatile long enc1Ticks = 0;
volatile long enc2Ticks = 0;

void isrEnc1() { enc1Ticks++; }
void isrEnc2() { enc2Ticks++; }

void setup() {
  Serial.begin(115200);

  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP2_PIN, OUTPUT);
  pinMode(DIR2_PIN, OUTPUT);

  pinMode(IN1_PIN, OUTPUT);
  pinMode(IN2_PIN, OUTPUT);
  pinMode(IN3_PIN, OUTPUT);
  pinMode(IN4_PIN, OUTPUT);

  pinMode(ENC1_PIN, INPUT_PULLUP);
  pinMode(ENC2_PIN, INPUT_PULLUP);
  // The Arduino 101 core accepts the pin number directly here.
  attachInterrupt(ENC1_PIN, isrEnc1, RISING);
  attachInterrupt(ENC2_PIN, isrEnc2, RISING);

  Serial.println("System Ready: 2 Steppers + 2 DC Motors + 2 Encoders");
}

void loop() {
  digitalWrite(IN1_PIN, HIGH);
  digitalWrite(IN2_PIN, LOW);
  digitalWrite(IN3_PIN, HIGH);
  digitalWrite(IN4_PIN, LOW);

  digitalWrite(DIR_PIN, HIGH);
  digitalWrite(DIR2_PIN, HIGH);

  for (int i = 0; i < 200; i++) {
    digitalWrite(STEP_PIN, HIGH);
    digitalWrite(STEP2_PIN, HIGH);
    delayMicroseconds(1000);
    digitalWrite(STEP_PIN, LOW);
    digitalWrite(STEP2_PIN, LOW);
    delayMicroseconds(1000);
  }

  noInterrupts();
  long ticks1 = enc1Ticks;
  long ticks2 = enc2Ticks;
  interrupts();

  Serial.print("Enc 1 Ticks: ");
  Serial.print(ticks1);
  Serial.print(" | Enc 2 Ticks: ");
  Serial.println(ticks2);
  delay(1000);
}
