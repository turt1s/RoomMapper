#include <Arduino.h>
#include <Servo.h>

// type an angle (0-180) into the serial monitor and press Enter,
// then the servo sweeps from where it is to that angle at a steady speed
//
// decimals work too (ex. 45.5): the angle goes out as a pulse width in
// microseconds, which gives ~0.1 degree steps instead of whole degrees,
// but the gears have ~3 degrees of backlash so don't expect that precision

const uint8_t SERVO_PIN = 12;
const float STEP_DEG = 0.5;    // degrees per step (smaller = smoother)
const int STEP_DELAY_MS = 8;   // pause per step (bigger = slower), ~2.9 s for 0 -> 180

// pulse widths the Servo library uses for 0 and 180 by default
const int MIN_US = 544;
const int MAX_US = 2400;

Servo servo;
float currentAngle = 90;  // no position feedback, so we just remember what we last sent

char inputBuf[16];
uint8_t inputLen = 0;

void writeAngle(float angle) {
  int us = MIN_US + (int)(angle * (MAX_US - MIN_US) / 180.0 + 0.5);
  servo.writeMicroseconds(us);
}

void sweepTo(float target) {
  while (currentAngle != target) {
    if (abs(target - currentAngle) <= STEP_DEG) {
      currentAngle = target;  // last step lands exactly on the target
    } else if (target > currentAngle) {
      currentAngle += STEP_DEG;
    } else {
      currentAngle -= STEP_DEG;
    }
    writeAngle(currentAngle);
    delay(STEP_DELAY_MS);
  }
}

void handleInput(const char *text) {
  // toFloat() returns 0 for junk, so check it actually starts like a number
  if (!(isDigit(text[0]) || text[0] == '.')) {
    Serial.print(F("Not a number: "));
    Serial.println(text);
    return;
  }

  float target = String(text).toFloat();
  if (target < 0 || target > 180) {
    Serial.println(F("Angle must be 0 to 180."));
    return;
  }

  Serial.print(F("Sweeping "));
  Serial.print(currentAngle, 1);
  Serial.print(F(" -> "));
  Serial.println(target, 1);
  sweepTo(target);
  Serial.println(F("There. Enter another angle:"));
}

void setup() {
  Serial.begin(115200);
  servo.attach(SERVO_PIN);
  writeAngle(currentAngle);
  Serial.println(F("Servo at 90. Enter an angle 0-180 (decimals ok):"));
}

void loop() {
  // build up a line one character at a time so slow typing still works
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputLen > 0) {
        inputBuf[inputLen] = '\0';
        handleInput(inputBuf);
        inputLen = 0;
      }
    } else if (inputLen < sizeof(inputBuf) - 1) {
      inputBuf[inputLen++] = c;
    }
  }
}
