#include <Arduino.h>
#include <Wire.h>

const uint8_t MPU_ADDR = 0x68;

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

uint8_t readReg(uint8_t reg) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.endTransmission(false); // false keeps hold of the bus so the read follows straight on
  Wire.requestFrom(MPU_ADDR, (uint8_t)1);
  return Wire.read();
}

int16_t read16() {
  // two separate lines so hi is always read first; inside one expression
  // C++ is allowed to do the two Wire.read() calls in either order
  uint8_t hi = Wire.read();
  uint8_t lo = Wire.read();
  return (int16_t)((hi << 8) | lo);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  writeReg(0x6B, 0x00); // PWR_MGMT_1: the chip powers up asleep, so wake it
  delay(100);
  Serial.print("WHO_AM_I = 0x");
  Serial.println(readReg(0x75), HEX);
}

void loop() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H: start of the 14 data bytes
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, (uint8_t)14);

  int16_t ax = read16(), ay = read16(), az = read16();
  read16(); // skip temperature
  int16_t gx = read16(), gy = read16(), gz = read16();

  Serial.print(ax); Serial.print('\t');
  Serial.print(ay); Serial.print('\t');
  Serial.print(az); Serial.print('\t');
  Serial.print(gx); Serial.print('\t');
  Serial.print(gy); Serial.print('\t');
  Serial.println(gz);
  delay(100);
}
