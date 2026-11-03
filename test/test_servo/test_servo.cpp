#include <Arduino.h>
#include <unity.h>
#include <Servo.h>

#define SERVO_PIN 12

Servo testServo;

void setUp(void) {}

void tearDown(void) {}

void test_servo_attaches_to_pin(void) {
    testServo.attach(SERVO_PIN);
    TEST_ASSERT_TRUE_MESSAGE(testServo.attached(), "Servo did not attach to pin 12");
}

// A hobby servo has no position feedback. read() only returns the last
// value passed to write(), so these checks verify the commands we send, not
// where the horn actually is. The sweep test is the one you check by eye.

void test_servo_stores_commanded_angle(void) {
    if (!testServo.attached()) TEST_IGNORE_MESSAGE("skipped: servo not attached");
    testServo.write(90);
    TEST_ASSERT_EQUAL_INT_MESSAGE(90, testServo.read(), "Servo library did not store the commanded angle");
    delay(500); // let the horn reach centre before the next test
}

void test_servo_clamps_out_of_range(void) {
    if (!testServo.attached()) TEST_IGNORE_MESSAGE("skipped: servo not attached");
    testServo.write(-10);
    TEST_ASSERT_EQUAL_INT_MESSAGE(0, testServo.read(), "Negative angle was not clamped to 0");
    delay(500);
    testServo.write(170);
    TEST_ASSERT_EQUAL_INT_MESSAGE(170, testServo.read(), "Angle 170 was not stored");
    delay(500);
}

void test_servo_full_range_sweep(void) {
    if (!testServo.attached()) TEST_IGNORE_MESSAGE("skipped: servo not attached");
    // watch the horn: it should move smoothly 0 -> 180 -> 0 with no jitter or stalling
    for (int angle = 0; angle <= 180; angle += 5) {
        testServo.write(angle);
        delay(20);
    }
    for (int angle = 180; angle >= 0; angle -= 5) {
        testServo.write(angle);
        delay(20);
    }
    TEST_ASSERT_TRUE_MESSAGE(testServo.attached(), "Servo detached unexpectedly during the sweep");
}

void setup() {
    delay(2000); // give the serial monitor time to connect
    Serial.begin(115200);

    UNITY_BEGIN();
    RUN_TEST(test_servo_attaches_to_pin);
    RUN_TEST(test_servo_stores_commanded_angle);
    RUN_TEST(test_servo_clamps_out_of_range);
    RUN_TEST(test_servo_full_range_sweep);
    UNITY_END();

    testServo.detach();
}

void loop() {}
