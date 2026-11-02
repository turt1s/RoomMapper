# RoomMapper

A small Arduino robot that maps the room around it. A VL53L1X distance sensor on a servo sweeps side to side to measure how far away things are while the robot drives around.

Still a work in progress.

## Hardware

- Arduino Uno WiFi
- Adafruit VL53L1X time-of-flight sensor
- Servo
- MPU-6050
- L298N motor driver + 2 DC motors with encoders
- LM2596 buck converter
- 6× AA battery pack

![Wiring diagram](./docs/breadboard_diagram.png)

## Usage

Built with [PlatformIO](https://platformio.org/).

```sh
pio run -t upload       # build and upload
pio device monitor      # serial monitor (115200 baud)
```

Tests run on the board, so the hardware needs to be connected:

```sh
pio test -f test_servo
pio test -f test_vl53l1x
```

## Scratch sketches

`src/scratch/` holds small standalone sketches for poking at one piece of hardware at a time. They're left out of the main build. To run one, point the `scratch` env in `platformio.ini` at it:

```ini
[env:scratch]
extends = env:uno
build_src_filter = +<scratch/mpu_raw.cpp>
```

Then build, upload and open the monitor with that env:

```sh
pio run -e scratch -t upload
pio device monitor -e scratch
```

| Sketch | What it does |
| --- | --- |
| `blink.cpp` | Blinks the onboard LED to check the board is alive |
| `i2c_scan.cpp` | Prints the address of every device on the I2C bus |
| `mpu_raw.cpp` | Reads raw accelerometer and gyro values from the MPU-6050 |
