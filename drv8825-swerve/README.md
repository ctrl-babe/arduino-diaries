# DRV8825 Swerve Controller

Four-motor controller firmware with stepper-driven steering and DC motor drive. The project includes a PlatformIO configuration and an Arduino IDE wrapper sketch.

## Targets

PlatformIO defines `uno_side1`, `bluepill_side1`, and `bluepill_side2` environments. Select the environment that matches your board and the side of the robot you are controlling.

## Build

With PlatformIO Core installed:

```sh
pio run -e uno_side1
```

To build for a Blue Pill, use `-e bluepill_side1` or `-e bluepill_side2`. Set the correct serial port in the PlatformIO environment before uploading; no machine-specific port is committed.

For Arduino IDE, open `drv8825-swerve.ino` and choose the appropriate board. Review the source and hardware wiring before powering the motor drivers.

## Source layout

- `src/main.cpp` — board-specific setup, pin mapping, and controller logic
- `src/stepper_motor.*` — stepper motor control
- `src/dc_motor.*` — DC motor control
- `src/swerve_module.*` — swerve module behavior
- `src/encoder.h` — encoder interface

Check the source for the actual pin map and hardware assumptions before connecting motors.
