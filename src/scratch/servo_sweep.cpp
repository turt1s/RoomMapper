#include <Arduino.h>
#include <Servo.h>

// a hobby servo has no position feedback, so check this one by eye:
// the horn should sweep 0 -> 180 -> 0 smoothly with no jitter or stalling,
// then hold at 90 (centre) so it stays put while you mount things to it

const uint8_t SERVO_PIN = 12;
const int SWEEPS = 3;
const int STEP_DEG = 1;        // degrees per step (smaller = smoother)
const int STEP_DELAY_MS = 15;  // pause per step (bigger = slower), ~2.7 s per half-sweep

Servo servo;

void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN);

  for (int i = 0; i < SWEEPS; i++) {
    Serial.print(F("Sweep "));
    Serial.println(i + 1);
    for (int angle = 0; angle <= 180; angle += STEP_DEG) {
      servo.write(angle);
      delay(STEP_DELAY_MS);
    }
    for (int angle = 180; angle >= 0; angle -= STEP_DEG) {
      servo.write(angle);
      delay(STEP_DELAY_MS);
    }
  }

  Serial.println(F("Done, holding at 90."));
}

// keep sending 90 so the servo actively holds centre and resists being turned
void loop() {
  servo.write(90);
  delay(20);
}
