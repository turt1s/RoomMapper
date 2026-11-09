#include <Arduino.h>
#include <Servo.h>

// a hobby servo has no position feedback, so check this one by eye:
// the horn should sweep 0 -> 180 -> 0 smoothly with no jitter or stalling,
// then settle at 90 (centre)

const uint8_t SERVO_PIN = 12;
const int SWEEPS = 3;

Servo servo;

void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN);

  for (int i = 0; i < SWEEPS; i++) {
    Serial.print(F("Sweep "));
    Serial.println(i + 1);
    for (int angle = 0; angle <= 180; angle += 5) {
      servo.write(angle);
      delay(20);
    }
    for (int angle = 180; angle >= 0; angle -= 5) {
      servo.write(angle);
      delay(20);
    }
  }

  servo.write(90);
  delay(500); // let the horn reach centre before detaching
  servo.detach();
  Serial.println(F("Done, centred at 90."));
}

void loop() {}
