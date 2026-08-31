#include <Arduino.h>

void setup() {
  // We are forcing the brain to use Pin 13 specifically
  pinMode(13, OUTPUT);
}

void loop() {
  digitalWrite(13, HIGH);
  delay(2000);             // Fast 0.2 second delay
  digitalWrite(13, LOW);
  delay(2000);             // Fast 0.2 second delay
}