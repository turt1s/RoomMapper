#include <Arduino.h>

const uint8_t L_A = 2, L_B = 4;
const uint8_t R_A = 3, R_B = 5;

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

void setup() {
  Serial.begin(115200);

  pinMode(L_A, INPUT);
  pinMode(L_B, INPUT);
  pinMode(R_A, INPUT);
  pinMode(R_B, INPUT);

  attachInterrupt(digitalPinToInterrupt(L_A), leftISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(R_A), rightISR, CHANGE);

  Serial.println(F("left\tright"));
}

void loop() {
  // a long is read 1 byte at a time, so pause interrupts to avoid a torn value
  noInterrupts();
  long l = leftCount;
  long r = rightCount;
  interrupts();

  Serial.print(l);
  Serial.print('\t');
  Serial.println(r);
  delay(100);
}
