/**
 * Arduino IDE wrapper entry point for 4-Motor Controller (Arduino Uno / STM32 BluePill).
 * 
 * If using Arduino IDE:
 * FOR SIDE 1 (Arduino Uno):
 * 1. Select Board: "Arduino Uno"
 * 2. Select Port & Upload directly!
 * 
 * FOR SIDE 2 (STM32 BluePill):
 * 1. Install "STM32 MCU based boards" in Board Manager.
 * 2. Select Board: "Generic STM32F1 series" -> Part Number: "BluePill F103C8".
 * 3. Select Upload method: "STM32CubeProgrammer (Serial)" or "Serial".
 */

#include "src/dc_motor.h"
#include "src/stepper_motor.h"

// Include main implementation logic directly for Arduino IDE build system compatibility
#include "src/dc_motor.cpp"
#include "src/stepper_motor.cpp"
#include "src/main.cpp"

