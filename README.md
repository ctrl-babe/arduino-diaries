# Arduino Diaries

> A collection of Arduino sketches and embedded robotics projects, from RFID experiments to swerve-drive controllers.

Browse the project folders for sketches, PlatformIO configurations, and notes on getting started.

## Projects

| Project | What it does | Start here |
| --- | --- | --- |
| RFID Attendance | RFID card check-in prototype with an OLED display and status indicators. | [`rfid-attendance/`](./rfid-attendance/) |
| DRV8825 Swerve Controller | Four-motor controller with stepper steering and DC drive modules; includes Arduino IDE and PlatformIO entry points. | [`drv8825-swerve/`](./drv8825-swerve/) |
| HK32 Swerve Controller | PlatformIO firmware targeting an HK32F103C8-compatible board. | [`hk32-swerve/`](./hk32-swerve/) |
| Swerve 101 | A standalone Arduino sketch for an early swerve-drive prototype. | [`swerve-101/`](./swerve-101/) |
| HK32 Swerve Reference Sketch | A standalone reference sketch for the HK32 swerve controller. | [`hk32-swerve-reference/`](./hk32-swerve-reference/) |

## Getting started

1. Open a project's README for its board, libraries, and setup notes.
2. Open the `.ino` file in Arduino IDE, or open a PlatformIO project folder in VS Code.
3. Select the correct board and serial port for your hardware before compiling or uploading.

Hardware and pin assignments are project-specific. Check the source and verify wiring before connecting power.

## Repository layout

Each project is kept in its own folder so sketches, firmware, and build configuration stay together. Generated build output and machine-specific settings are intentionally excluded.
