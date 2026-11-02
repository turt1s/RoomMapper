#include <Arduino.h>
#include "Adafruit_VL53L1X.h"

// XSHUT and IRQ aren't wired -- dataReady() polls over I2C instead
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Serial.println(F("Adafruit VL53L1X sensor demo"));

  Wire.begin();
  if (!vl53.begin(0x29, &Wire)) { // 0x29 is the default address
    Serial.print(F("Error on init of VL sensor: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }
  Serial.println(F("VL53L1X sensor OK!"));

  Serial.print(F("Sensor ID: 0x"));
  Serial.println(vl53.sensorID(), HEX);

  if (!vl53.startRanging()) {
    Serial.print(F("Couldn't start ranging: "));
    Serial.println(vl53.vl_status);
    while (1) delay(10);
  }
  Serial.println(F("Ranging started"));

  // valid timing budgets: 15, 20, 33, 50, 100, 200, 500 ms
  vl53.setTimingBudget(50);
  Serial.print(F("Timing budget (ms): "));
  Serial.println(vl53.getTimingBudget());
}

void loop() {
  int16_t distance;

  if (vl53.dataReady()) {
    distance = vl53.distance();
    if (distance == -1) {
      Serial.print(F("Couldn't get distance: "));
      Serial.println(vl53.vl_status);
      return;
    }
    Serial.print(F("Distance: "));
    Serial.print(distance);
    Serial.println(" mm");

    vl53.clearInterrupt();
  }
}
