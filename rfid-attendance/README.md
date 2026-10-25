# RFID Attendance

An Arduino attendance/check-in prototype using an MFRC522 reader, a 128×64 OLED, and buzzer/LED feedback.

## Hardware and libraries

- Arduino-compatible board (the sketch includes notes for Uno and STM32 Blue Pill)
- MFRC522 RFID reader and compatible tag
- 128×64 SH1106 OLED display
- Buzzer and green, red, and yellow indicator LEDs
- Arduino libraries: MFRC522, Adafruit GFX Library, and Adafruit SH110X

The sketch uses SPI for the RFID reader and I²C for the display. Confirm the pin definitions in `control_de_asistencia_rfid.ino` match your wiring.

## Setup

1. Install the libraries listed above through Arduino IDE's Library Manager.
2. Open `rfid-attendance.ino` in Arduino IDE and select your board and port.
3. Set `blueUID` to the UID of your own test tag. The published sketch uses a zeroed placeholder; do not commit a real access-card UID.
4. Compile, then upload and test with a non-sensitive tag.

## Notes

This is a prototype, not a secure access-control system. RFID UIDs can be copied or emulated; do not use UID-only checks to protect attendance records, doors, or other sensitive resources.
