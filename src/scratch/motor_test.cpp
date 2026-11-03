#include <Arduino.h>
#include <avr/wdt.h>

const uint8_t L_A = 2, L_B = 4;
const uint8_t R_A = 3, R_B = 5;

//left motor’s leads are reversed intentionally
const uint8_t L_EN = 11,  L_FWD = 10, L_REV = 9; //L298N ENB, IN4, IN3
const uint8_t R_EN = 6, R_FWD = 7, R_REV = 8;    //L298N ENA, IN1, IN2

const int TEST_PWM = 120;

volatile long leftCount = 0;
volatile long rightCount = 0;

void leftISR() {
  if (digitalRead(L_A) == digitalRead(L_B)) leftCount++;
  else                                      leftCount--;
}

// right motor is mounted flipped, so its direction is reversed
void rightISR() {
  if (digitalRead(R_A) == digitalRead(R_B)) rightCount--;
  else                                      rightCount++;
}

void setLeft(int speed) {
  digitalWrite(L_FWD, speed > 0);
  digitalWrite(L_REV, speed < 0);
  analogWrite(L_EN, constrain(abs(speed), 0, 255));
}

void setRight(int speed) {
  digitalWrite(R_FWD, speed > 0);
  digitalWrite(R_REV, speed < 0);
  analogWrite(R_EN, constrain(abs(speed), 0, 255));
}

void stopMotors() {
  setLeft(0);
  setRight(0);
}

// motors off, then sit here forever; keep feeding the watchdog so it doesn't reset
void halt(const __FlashStringHelper *why) {
  stopMotors();
  Serial.println(why);
  while (true) wdt_reset();
}

// replaces delay(): feeds the watchdog and stops everything if 's' arrives
void safeDelay(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    wdt_reset();
    if (Serial.available() && Serial.read() == 's') halt(F("STOPPED by user."));
  }
}

void runStep(const __FlashStringHelper *name, int left, int right) {
  Serial.println(name);
  setLeft(left);
  setRight(right);

  for (int i = 0; i < 10; i++) {
    // a long is read 1 byte at a time, so pause interrupts to avoid a torn value
    noInterrupts();
    long l = leftCount;
    long r = rightCount;
    interrupts();

    Serial.print(l);
    Serial.print('\t');
    Serial.println(r);
    safeDelay(200);
  }

  stopMotors();
  safeDelay(1000);
}

void setup() {
  wdt_disable();  // in case we got here from a watchdog reset

  // motor pins first, so they're driven LOW (off) as early as possible
  pinMode(L_EN, OUTPUT);
  pinMode(L_FWD, OUTPUT);
  pinMode(L_REV, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(R_FWD, OUTPUT);
  pinMode(R_REV, OUTPUT);
  stopMotors();

  Serial.begin(115200);

  pinMode(L_A, INPUT);
  pinMode(L_B, INPUT);
  pinMode(R_A, INPUT);
  pinMode(R_B, INPUT);
  attachInterrupt(digitalPinToInterrupt(L_A), leftISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(R_A), rightISR, CHANGE);

  Serial.println(F("Motors OFF. Type g + Enter to start, s + Enter to stop."));
  while (!(Serial.available() && Serial.read() == 'g')) { }

  wdt_enable(WDTO_500MS);  // a hang longer than 0.5 s now resets the chip

  runStep(F("LEFT forward"),   TEST_PWM, 0);
  runStep(F("LEFT backward"), -TEST_PWM, 0);
  runStep(F("RIGHT forward"),  0,  TEST_PWM);
  runStep(F("RIGHT backward"), 0, -TEST_PWM);

  halt(F("Done."));
}

void loop() {}