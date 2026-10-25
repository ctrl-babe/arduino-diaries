#ifndef STEPPER_MOTOR_H
#define STEPPER_MOTOR_H

#include <Arduino.h>

/**
 * @brief Non-blocking DRV8825 Stepper Motor Driver class for STM32
 */
class StepperMotor {
private:
    uint8_t _stepPin;
    uint8_t _dirPin;
    uint8_t _enablePin;
    bool _hasEnablePin;

    long _targetSteps;       // Total steps left to complete
    uint32_t _stepIntervalUs; // Delay between step pulse toggles in microseconds
    uint32_t _lastStepTimeUs; // Timestamp of last step pulse
    bool _stepState;          // Current step pin state (HIGH/LOW)
    bool _direction;          // Current direction state (true=CW/FORWARD, false=CCW/REVERSE)
    bool _enabled;            // Driver enable state

public:
    /**
     * @brief Construct a new StepperMotor object
     * @param stepPin STEP pin connected to DRV8825
     * @param dirPin DIR pin connected to DRV8825
     * @param enablePin (Optional) ENABLE pin connected to DRV8825 (Active LOW)
     */
    StepperMotor(uint8_t stepPin, uint8_t dirPin, int8_t enablePin = -1);

    /**
     * @brief Initialize pins and reset internal timers
     */
    void begin();

    /**
     * @brief Set direction explicitly
     * @param forward true for HIGH on DIR, false for LOW
     */
    void setDirection(bool forward);

    /**
     * @brief Command non-blocking relative step movement
     * @param steps Number of steps (positive = forward, negative = reverse)
     * @param stepIntervalUs Delay between step pulses in microseconds (minimum ~10us)
     */
    void move(long steps, uint32_t stepIntervalUs);

    /**
     * @brief Command step movement using RPM speed
     * @param steps Number of steps
     * @param rpm Desired speed in RPM
     * @param stepsPerRev Steps per full motor revolution (default: 200 for 1.8 deg stepper)
     * @param microstepping Microstepping multiplier (1 = full step, 2 = 1/2, 16 = 1/16, 32 = 1/32)
     */
    void moveRpm(long steps, float rpm, uint16_t stepsPerRev = 200, uint8_t microstepping = 1);

    /**
     * @brief Must be called repeatedly inside main loop() to pulse STEP pin
     * @return true if step was executed on this tick, false otherwise
     */
    bool run();

    /**
     * @brief Enable DRV8825 driver outputs (DRV8825 EN pin pulled LOW)
     */
    void enable();

    /**
     * @brief Disable DRV8825 driver outputs (DRV8825 EN pin pulled HIGH to prevent idle heating)
     */
    void disable();

    /**
     * @brief Check if stepper is currently performing a move
     * @return true if steps remaining > 0
     */
    bool isMoving() const { return _targetSteps > 0; }

    /**
     * @brief Stop remaining step count immediately
     */
    void stop();
};

#endif // STEPPER_MOTOR_H
