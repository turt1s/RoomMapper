#include <Arduino.h>
#include <unity.h>
#include "Adafruit_VL53L1X.h"

// rated for about 40-4000 mm, but close objects can read under 40,
// so anything above 0 counts as a real reading
#define MIN_VALID_MM 1
#define MAX_VALID_MM 4000

// XSHUT and IRQ aren't wired, so dataReady() polls over I2C instead
Adafruit_VL53L1X vl53 = Adafruit_VL53L1X();

bool sensorReady = false;
bool rangingStarted = false;

void setUp(void) {}

void tearDown(void) {}

void test_sensor_init(void) {
    sensorReady = vl53.begin(0x29, &Wire);
    TEST_ASSERT_TRUE_MESSAGE(sensorReady, "VL53L1X sensor initialization failed -- check SDA/SCL/VIN/GND wiring");
}

void test_sensor_id(void) {
    if (!sensorReady) TEST_IGNORE_MESSAGE("skipped: sensor not initialized");
    uint16_t sensor_id = vl53.sensorID();
    TEST_ASSERT_EQUAL_HEX16_MESSAGE(0xEACC, sensor_id, "VL53L1X sensor ID mismatch");
}

void test_start_ranging(void) {
    if (!sensorReady) TEST_IGNORE_MESSAGE("skipped: sensor not initialized");
    rangingStarted = vl53.startRanging();
    TEST_ASSERT_TRUE_MESSAGE(rangingStarted, "VL53L1X ranging failed");
}

void test_distance_reading_valid(void) {
    if (!rangingStarted) TEST_IGNORE_MESSAGE("skipped: ranging not started");

    unsigned long start = millis();
    while (!vl53.dataReady()) {
        if (millis() - start > 2000) {
            TEST_FAIL_MESSAGE("Timed out waiting for data");
        }
        delay(10);
    }
    int16_t distance = vl53.distance();
    vl53.clearInterrupt();

    TEST_ASSERT_NOT_EQUAL_MESSAGE(-1, distance, "distance() returned error (-1)");
    // needs a target within about 4 m (a wall, your hand)
    TEST_ASSERT_INT16_WITHIN_MESSAGE((MAX_VALID_MM - MIN_VALID_MM) / 2, (MAX_VALID_MM + MIN_VALID_MM) / 2, distance,
                                     "Distance out of range -- is there a target within 4m?");
}

void setup() {
    delay(2000); // give the serial monitor time to connect
    Serial.begin(115200);

    Wire.begin();

    UNITY_BEGIN();
    RUN_TEST(test_sensor_init);
    RUN_TEST(test_sensor_id);
    RUN_TEST(test_start_ranging);
    RUN_TEST(test_distance_reading_valid);
    UNITY_END();

    if (rangingStarted) vl53.stopRanging();
}

void loop() {}
