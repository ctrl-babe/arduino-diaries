#include <Arduino.h>
#include "swerve_module.h"

// ============================================================================
// BOARD SELECTION & PIN MAPPING SCHEME
// ============================================================================

// Toggle target board configuration:
// Select SIDE 1 (Arduino Uno or STM32 BluePill) or SIDE 2 (STM32 BluePill)

#if defined(BOARD_UNO_SIDE1) || defined(ARDUINO_AVR_UNO) || defined(UNO_SIDE_1)
  #ifndef BOARD_NAME
    #define BOARD_NAME "ARDUINO UNO SIDE 1 (FRONT MODULES 1 & 2)"
  #endif
  #define MOD_A_ID 1
  #define MOD_B_ID 2

  // Module 1 (Front Left) Pin Definitions (Arduino Uno)
  #define MOD_A_STEP    4
  #define MOD_A_DIR     5
  #define MOD_A_EN      6
  #define MOD_A_PWM     -1    // ENA jumper shorted to 5V (Full speed)
  #define MOD_A_IN1     7
  #define MOD_A_IN2     8
  #define MOD_A_ENC_A   2     // Hardware Interrupt INT0 (Pin 2)
  #define MOD_A_ENC_B   9

  // Module 2 (Front Right) Pin Definitions (Arduino Uno)
  #define MOD_B_STEP    10
  #define MOD_B_DIR     11
  #define MOD_B_EN      12
  #define MOD_B_PWM     -1    // ENB jumper shorted to 5V (Full speed)
  #define MOD_B_IN1     A0    // Digital Output via Analog Pin A0
  #define MOD_B_IN2     A1    // Digital Output via Analog Pin A1
  #define MOD_B_ENC_A   3     // Hardware Interrupt INT1 (Pin 3)
  #define MOD_B_ENC_B   A2    // Digital Input via Analog Pin A2

#elif defined(BOARD_BLUEPILL_SIDE1) || defined(BLUEPILL_SIDE_1) || defined(SIDE_1)
  #ifndef BOARD_NAME
    #define BOARD_NAME "STM32 BLUEPILL SIDE 1 (FRONT MODULES 1 & 2)"
  #endif
  #define MOD_A_ID 1
  #define MOD_B_ID 2

  // Module 1 (Front Left) Pin Definitions
  #define MOD_A_STEP    PA2
  #define MOD_A_DIR     PA3
  #define MOD_A_EN      PA4
  #define MOD_A_PWM     -1    // -1: Jumper attached on ENA (ON/OFF full speed mode)
  #define MOD_A_IN1     PB12
  #define MOD_A_IN2     PB13
  #define MOD_A_ENC_A   PB4   // EXTI4
  #define MOD_A_ENC_B   PB5

  // Module 2 (Front Right) Pin Definitions
  #define MOD_B_STEP    PA5
  #define MOD_B_DIR     PA6
  #define MOD_B_EN      PA7
  #define MOD_B_PWM     -1    // -1: Jumper attached on ENB (ON/OFF full speed mode)
  #define MOD_B_IN1     PB14
  #define MOD_B_IN2     PB15
  #define MOD_B_ENC_A   PB6   // EXTI6
  #define MOD_B_ENC_B   PB7

#else // Default: BOARD_BLUEPILL_SIDE2 (Rear Modules 3 & 4)
  #ifndef BOARD_NAME
    #define BOARD_NAME "STM32 BLUEPILL SIDE 2 (REAR MODULES 3 & 4)"
  #endif
  #define MOD_A_ID 3
  #define MOD_B_ID 4

  // Module 3 (Rear Left) Pin Definitions
  #define MOD_A_STEP    PA2
  #define MOD_A_DIR     PA3
  #define MOD_A_EN      PA4
  #define MOD_A_PWM     -1    // -1: Jumper attached on ENA (ON/OFF full speed mode)
  #define MOD_A_IN1     PB12
  #define MOD_A_IN2     PB13
  #define MOD_A_ENC_A   PB4   // EXTI4
  #define MOD_A_ENC_B   PB5

  // Module 4 (Rear Right) Pin Definitions
  #define MOD_B_STEP    PA5
  #define MOD_B_DIR     PA6
  #define MOD_B_EN      PA7
  #define MOD_B_PWM     -1    // -1: Jumper attached on ENB (ON/OFF full speed mode)
  #define MOD_B_IN1     PB14
  #define MOD_B_IN2     PB15
  #define MOD_B_ENC_A   PB6   // EXTI6
  #define MOD_B_ENC_B   PB7
#endif

// ============================================================================
// INSTANTIATE DUAL SWERVE MODULES FOR THIS CONTROLLER
// ============================================================================
SwerveModule moduleA(MOD_A_ID, MOD_A_STEP, MOD_A_DIR, MOD_A_EN,
                     MOD_A_PWM, MOD_A_IN1, MOD_A_IN2,
                     MOD_A_ENC_A, MOD_A_ENC_B, 200, 16); // 200 steps/rev @ 1/16 microstep

SwerveModule moduleB(MOD_B_ID, MOD_B_STEP, MOD_B_DIR, MOD_B_EN,
                     MOD_B_PWM, MOD_B_IN1, MOD_B_IN2,
                     MOD_B_ENC_A, MOD_B_ENC_B, 200, 16); // 200 steps/rev @ 1/16 microstep

// Standalone ISR Trampolines for Hall Encoders
void isrModuleA() { moduleA.handleEncoderInterrupt(); }
void isrModuleB() { moduleB.handleEncoderInterrupt(); }

// State & Serial Variables
uint32_t lastTelemetryTime = 0;
bool liveTelemetry = false;
String inputString = "";
bool stringComplete = false;

// Function Prototypes
void printHelp();
void runAutoDiagnostic();
void processCommand(String cmd);

// ============================================================================
// SETUP
// ============================================================================
void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println(F("\n=================================================="));
    Serial.print(F(" SYSTEM INITIALIZING: ")); Serial.println(F(BOARD_NAME));
    Serial.println(F(" Dual Swerve Wheel Controller Framework Loaded"));
    Serial.println(F(" ENA & ENB Mode: Jumper ON (Full Speed Control)"));
    Serial.println(F("=================================================="));

    // Initialize Module A and Module B
    moduleA.begin(isrModuleA);
    moduleB.begin(isrModuleB);

    inputString.reserve(64);
    printHelp();

    Serial.println(F("\n>> Ready! Type 'TEST' to run automatic diagnostic motion routine."));
}

// ============================================================================
// MAIN LOOP (Non-blocking Stepper Service, Telemetry & CLI Parser)
// ============================================================================
void loop() {
    // 1. Service non-blocking stepper pulse engine for both modules
    moduleA.update();
    moduleB.update();

    // 2. Periodic Live Telemetry Output (every 100ms)
    uint32_t now = millis();
    if (now - lastTelemetryTime >= 100) {
        lastTelemetryTime = now;
        if (liveTelemetry || moduleA.getDriveSpeed() != 0 || moduleB.getDriveSpeed() != 0 ||
            moduleA.isSteeringMoving() || moduleB.isSteeringMoving()) {
            
            Serial.print(F("[TELEMETRY] M")); Serial.print(MOD_A_ID);
            Serial.print(F(" (Steer:")); Serial.print(moduleA.isSteeringMoving() ? "BUSY" : "IDLE");
            Serial.print(F(" | DC:")); Serial.print(moduleA.getDriveSpeed());
            Serial.print(F(" | Ticks:")); Serial.print(moduleA.getEncoderTicks());
            Serial.print(F(" | RPM:")); Serial.print(moduleA.getDriveRPM(330), 1);

            Serial.print(F(") || M")); Serial.print(MOD_B_ID);
            Serial.print(F(" (Steer:")); Serial.print(moduleB.isSteeringMoving() ? "BUSY" : "IDLE");
            Serial.print(F(" | DC:")); Serial.print(moduleB.getDriveSpeed());
            Serial.print(F(" | Ticks:")); Serial.print(moduleB.getEncoderTicks());
            Serial.print(F(" | RPM:")); Serial.print(moduleB.getDriveRPM(330), 1);
            Serial.println(F(")"));
        }
    }

    // 3. Read Incoming Serial CLI Data
    while (Serial.available()) {
        char inChar = (char)Serial.read();
        if (inChar == '\n' || inChar == '\r') {
            if (inputString.length() > 0) {
                stringComplete = true;
            }
        } else {
            inputString += inChar;
        }
    }

    // 4. Execute Completed Command
    if (stringComplete) {
        inputString.trim();
        inputString.toUpperCase();
        processCommand(inputString);
        inputString = "";
        stringComplete = false;
    }
}

// ============================================================================
// AUTOMATIC DIAGNOSTIC MOTION TEST
// ============================================================================
void runAutoDiagnostic() {
    Serial.println(F("\n=================================================="));
    Serial.println(F(">> STARTING AUTOMATIC DIAGNOSTIC TEST..."));
    Serial.println(F("   1. Turning Steering Stepper M1 (+90 deg)"));
    Serial.println(F("   2. Running DC Motor M1 Forward"));
    Serial.println(F("   3. Turning Steering Stepper M2 (+90 deg)"));
    Serial.println(F("   4. Running DC Motor M2 Forward"));
    Serial.println(F("=================================================="));

    // Enable Telemetry
    liveTelemetry = true;

    // Step 1: Turn Module A steering 90 deg
    Serial.print(F(">> Rotating Steering Module ")); Serial.print(MOD_A_ID); Serial.println(F(" by 90 deg..."));
    moduleA.rotateSteering(90.0f, 30.0f);
    while (moduleA.isSteeringMoving()) {
        moduleA.update();
        delay(1);
    }

    // Step 2: Spin DC Motor A
    Serial.print(F(">> Spinning DC Motor Module ")); Serial.print(MOD_A_ID); Serial.println(F(" FORWARD..."));
    moduleA.setDriveSpeed(255);
    uint32_t tStart = millis();
    while (millis() - tStart < 2000) { // Run for 2 seconds
        moduleA.update();
        moduleB.update();
        delay(10);
    }
    moduleA.setDriveSpeed(0);

    // Step 3: Turn Module B steering 90 deg
    Serial.print(F(">> Rotating Steering Module ")); Serial.print(MOD_B_ID); Serial.println(F(" by 90 deg..."));
    moduleB.rotateSteering(90.0f, 30.0f);
    while (moduleB.isSteeringMoving()) {
        moduleB.update();
        delay(1);
    }

    // Step 4: Spin DC Motor B
    Serial.print(F(">> Spinning DC Motor Module ")); Serial.print(MOD_B_ID); Serial.println(F(" FORWARD..."));
    moduleB.setDriveSpeed(255);
    tStart = millis();
    while (millis() - tStart < 2000) { // Run for 2 seconds
        moduleA.update();
        moduleB.update();
        delay(10);
    }
    moduleB.setDriveSpeed(0);

    Serial.println(F(">> DIAGNOSTIC TEST COMPLETE! Both modules verified successfully.\n"));
}

// ============================================================================
// CLI INTERACTIVE CONTROL MENU
// ============================================================================
void printHelp() {
    Serial.println(F("\n--- SWERVE DUAL MODULE COMMAND MENU ---"));
    Serial.print(F(" Controller Target : ")); Serial.println(F(BOARD_NAME));
    Serial.println(F(" TEST                              : Run automatic 4-motor self diagnostic test!"));
    Serial.println(F(" HELP                              : Show this menu"));
    Serial.println(F(" STOP                              : Emergency stop both modules"));
    Serial.println(F(" DC <mod_id> <speed>               : Set DC motor (255=ON, 0=OFF) (e.g. DC 1 255)"));
    Serial.println(F(" STEP <mod_id> <steps> <us_delay>  : Stepper step test (e.g. STEP 1 1000 800)"));
    Serial.println(F(" ANGLE <mod_id> <deg> [rpm]        : Rotate steering angle (e.g. ANGLE 1 90 40)"));
    Serial.println(F(" SWERVE <mod_id> <deg> <speed>     : Run module steering + drive simultaneously"));
    Serial.println(F(" SWERVE_ALL <deg> <speed>          : Run BOTH modules on this board simultaneously"));
    Serial.println(F(" TELEMETRY <1/0>                   : Enable/Disable continuous 100ms telemetry"));
    Serial.println(F(" RESET_ENC                         : Reset encoder tick counts"));
    Serial.println(F("---------------------------------------\n"));
}

void processCommand(String cmd) {
    if (cmd == "HELP") {
        printHelp();
    } else if (cmd == "TEST") {
        runAutoDiagnostic();
    } else if (cmd == "STOP") {
        moduleA.stop();
        moduleB.stop();
        Serial.println(F(">> EMERGENCY STOP EXECUTE: All motors halted on this board."));
    } else if (cmd == "RESET_ENC") {
        moduleA.resetEncoder();
        moduleB.resetEncoder();
        Serial.println(F(">> Encoder ticks reset to 0 for both modules."));
    } else if (cmd.startsWith("TELEMETRY ")) {
        int val = cmd.substring(10).toInt();
        liveTelemetry = (val == 1);
        Serial.print(F(">> Telemetry output: "));
        Serial.println(liveTelemetry ? "ENABLED" : "DISABLED");
    } else if (cmd.startsWith("DC ")) {
        int p1 = cmd.indexOf(' ');
        int p2 = cmd.indexOf(' ', p1 + 1);
        if (p2 > 0) {
            uint8_t id = cmd.substring(p1 + 1, p2).toInt();
            int speed = cmd.substring(p2 + 1).toInt();
            if (id == MOD_A_ID) moduleA.setDriveSpeed(speed);
            else if (id == MOD_B_ID) moduleB.setDriveSpeed(speed);
            else Serial.println(F("Invalid module ID for this board!"));
        } else {
            Serial.println(F("Usage: DC <mod_id> <speed> (e.g. DC 1 255)"));
        }
    } else if (cmd.startsWith("STEP ")) {
        int p1 = cmd.indexOf(' ');
        int p2 = cmd.indexOf(' ', p1 + 1);
        int p3 = cmd.indexOf(' ', p2 + 1);
        if (p3 > 0) {
            uint8_t id = cmd.substring(p1 + 1, p2).toInt();
            long steps = cmd.substring(p2 + 1, p3).toInt();
            uint32_t delayUs = cmd.substring(p3 + 1).toInt();
            if (id == MOD_A_ID) moduleA.moveSteeringSteps(steps, delayUs);
            else if (id == MOD_B_ID) moduleB.moveSteeringSteps(steps, delayUs);
            else Serial.println(F("Invalid module ID for this board!"));
        } else {
            Serial.println(F("Usage: STEP <mod_id> <steps> <us_delay>"));
        }
    } else if (cmd.startsWith("ANGLE ")) {
        int p1 = cmd.indexOf(' ');
        int p2 = cmd.indexOf(' ', p1 + 1);
        int p3 = cmd.indexOf(' ', p2 + 1);
        if (p2 > 0) {
            uint8_t id = cmd.substring(p1 + 1, p2).toInt();
            float deg = p3 > 0 ? cmd.substring(p2 + 1, p3).toFloat() : cmd.substring(p2 + 1).toFloat();
            float rpm = p3 > 0 ? cmd.substring(p3 + 1).toFloat() : 30.0f;
            if (id == MOD_A_ID) moduleA.rotateSteering(deg, rpm);
            else if (id == MOD_B_ID) moduleB.rotateSteering(deg, rpm);
            else Serial.println(F("Invalid module ID for this board!"));
        } else {
            Serial.println(F("Usage: ANGLE <mod_id> <degrees> [rpm]"));
        }
    } else if (cmd.startsWith("SWERVE ")) {
        int p1 = cmd.indexOf(' ');
        int p2 = cmd.indexOf(' ', p1 + 1);
        int p3 = cmd.indexOf(' ', p2 + 1);
        if (p3 > 0) {
            uint8_t id = cmd.substring(p1 + 1, p2).toInt();
            float deg = cmd.substring(p2 + 1, p3).toFloat();
            int speed = cmd.substring(p3 + 1).toInt();
            if (id == MOD_A_ID) {
                moduleA.rotateSteering(deg, 35.0f);
                moduleA.setDriveSpeed(speed);
            } else if (id == MOD_B_ID) {
                moduleB.rotateSteering(deg, 35.0f);
                moduleB.setDriveSpeed(speed);
            } else {
                Serial.println(F("Invalid module ID for this board!"));
            }
        } else {
            Serial.println(F("Usage: SWERVE <mod_id> <degrees> <speed>"));
        }
    } else if (cmd.startsWith("SWERVE_ALL ")) {
        int p1 = cmd.indexOf(' ');
        int p2 = cmd.indexOf(' ', p1 + 1);
        if (p2 > 0) {
            float deg = cmd.substring(p1 + 1, p2).toFloat();
            int speed = cmd.substring(p2 + 1).toInt();
            moduleA.rotateSteering(deg, 35.0f);
            moduleA.setDriveSpeed(speed);
            moduleB.rotateSteering(deg, 35.0f);
            moduleB.setDriveSpeed(speed);
            Serial.println(F(">> Executing simultaneous swerve motion on BOTH modules!"));
        } else {
            Serial.println(F("Usage: SWERVE_ALL <degrees> <speed>"));
        }
    } else {
        Serial.println(F("Unknown command. Type 'HELP' for available CLI commands."));
    }
}
