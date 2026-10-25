# HK32 Swerve Controller

PlatformIO firmware for an HK32F103C8-compatible board using the Arduino framework.

## Build and upload

Open this folder in VS Code with PlatformIO installed, then build the `hk32f103c8_ftdi` environment:

```sh
pio run -e hk32f103c8_ftdi
```

Configure the upload port locally in PlatformIO before uploading. The project configuration intentionally does not pin a serial port because COM numbers vary between computers.

Review `src/main.cpp` for the firmware's board-specific pin assignments and wiring assumptions.
