# Obstacle-Avoiding Robot (ESP32)

An autonomous two-wheeled robot that measures the distance to obstacles with an ultrasonic sensor and reacts in three levels: it drives forward when the path is clear, warns when something is getting close, and stops and turns away when an obstacle is too near. LEDs, a buzzer and serial log messages show what the robot is "thinking" at every moment.

The whole behaviour is implemented in a single Arduino sketch, [`robot.ino`](robot.ino).

## About the Project

This robot was built as a course project. It combines sensor input, motor control and user feedback on a microcontroller in one autonomous system.

## What the Robot Does

The robot continuously reads the distance in front of it and chooses one of three states:

| State | Distance | LED | Buzzer | Motors | Serial message |
|---|---|---|---|---|---|
| **Safe** | more than 40 cm | Green | Off | Forward | `SAFE - forward` |
| **Warning** | 20–40 cm | Yellow | Off | Forward | `WARNING - obstacle ahead` |
| **Critical** | 20 cm or less | Red | On (500 ms) | Stop, then turn right for 650 ms | `CRITICAL - stopping!` |

On power-up it also runs a short **self-test**: it flashes the green, yellow and red LEDs one after another and beeps the buzzer, so you can check the wiring before the robot starts moving.

## Hardware

The components below are the ones driven by the code.

| Component | Role |
|---|---|
| ESP32 development board (on an extension board) | Main controller. The sketch uses ESP32 GPIO numbers such as 32 and 33. |
| HC-SR04 ultrasonic distance sensor | Measures the distance to obstacles |
| L293D motor driver | Drives the two DC motors in both directions |
| 2 × DC motors | Left and right wheels (differential drive) |
| Green, yellow and red LEDs | Show the current state |
| Buzzer | Audible warning in the critical state |
| Physical on/off switch | Lets the robot start moving once the self-test has finished |

### Pin Mapping

| Signal | GPIO | Notes |
|---|---|---|
| `TRIG` | 32 | Ultrasonic trigger (output) |
| `ECHO` | 33 | Ultrasonic echo (input) |
| `LED_G` | 13 | Green LED |
| `LED_Y` | 12 | Yellow LED |
| `LED_R` | 14 | Red LED |
| `BUZ` | 27 | Buzzer |
| `EN1` | 26 | L293D enable, left motor |
| `IN1` / `IN2` | 25 / 19 | Left motor direction |
| `EN2` | 18 | L293D enable, right motor |
| `IN3` / `IN4` | 4 / 2 | Right motor direction |

The header of the sketch describes this as a *"clean pin version"*: every LED, the buzzer and every motor-driver input has its own dedicated pin, so no pins are shared between components.

## How It Works

### 1. Measuring distance

`getDistance()` sends a 10 µs pulse on `TRIG` and measures how long `ECHO` stays high with `pulseIn()`. The time is converted to centimetres using the speed of sound:

```cpp
distance_cm = (duration_us * 0.0343) / 2.0;   // divide by 2: the sound travels there and back
```

`pulseIn()` is given a 30 ms timeout. If no echo arrives, the function returns `999`, which the rest of the program treats as "nothing in front of the robot". This stops a missing echo from blocking the loop.

### 2. Driving the motors

The L293D enable pins (`EN1`, `EN2`) are set `HIGH` once in `setup()`, so the motors always run at full speed. Direction is controlled with the four `IN` pins:

| Function | Left motor (`IN1`, `IN2`) | Right motor (`IN3`, `IN4`) |
|---|---|---|
| `goForward()` | forward | forward |
| `turnRight()` | forward | backward |
| `turnLeft()` | backward | forward |
| `goStop()` | off | off |

Turning on the spot works by spinning the two wheels in opposite directions. `turnLeft()` is implemented but not used by the current logic.

### 3. Main loop

Every cycle (about every 80 ms):

1. `allOff()` switches off all LEDs, the buzzer and the motor inputs.
2. The distance is measured and printed to the serial monitor.
3. The distance is compared with the thresholds `DIST_CLEAR` (40 cm) and `DIST_WARNING` (20 cm), and the matching state is applied.
4. In the critical state the robot stops, sounds the buzzer for 500 ms and then turns right for `TURN_TIME` (650 ms) before measuring again.

All thresholds and timings are constants at the top of the sketch, so they can be tuned without touching the logic. (`DIST_CRITICAL` = 10 cm is defined but not used yet.)

## Getting Started

### Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) with the **ESP32 board package** (Espressif) installed
- The hardware listed above, wired according to the pin table

### Upload

1. Clone the repository:
   ```bash
   git clone https://github.com/beatrizestfr/robot.git
   ```
2. Open `robot/robot.ino` in the Arduino IDE.
3. Select your ESP32 board and its COM port under **Tools**.
4. Click **Upload**.
5. Open the **Serial Monitor** at **115200 baud** to see the log.

### Example serial output

These are the messages the sketch prints:

```text
=== ROBOT STARTING ===
All good. Flip switch ON to run.
Distance: 85.3 cm  ->  SAFE - forward
Distance: 31.0 cm  ->  WARNING - obstacle ahead
Distance: 12.4 cm  ->  CRITICAL - stopping!
  -> turning to avoid
```

The distance values above are illustrative, not a recorded run. The switch in the startup message is the robot's physical on/off switch. It is part of the hardware and is not read by the code.

## What This Project Demonstrates

- Reading an ultrasonic sensor with precise timing (`delayMicroseconds`, `pulseIn` with a timeout)
- Controlling DC motors in both directions through an H-bridge driver (L293D)
- Designing simple state-based behaviour with clearly named thresholds
- Giving visual, audible and serial feedback to make the system easy to debug
- Planning GPIO assignments so that no pins are shared between components

## Possible Improvements

- Use **PWM** on the enable pins (`EN1`/`EN2`) to slow down in the warning zone instead of running at full speed
- Use the unused `DIST_CRITICAL` threshold, or turn left or right depending on where there is more space (`turnLeft()` already exists)
- Replace the blocking `delay()` calls with non-blocking timing (`millis()`) so the sensor keeps being read while turning
- Average several readings to filter out noisy measurements
- Add photos, a wiring diagram and a short video of the robot in action

## Project Status

A working demo: the latest commit adds the working demo code. The code is complete for the obstacle-avoidance behaviour described above. The ideas under *Possible Improvements* are not implemented.
