#ifndef SWERVE_MODULE_H
#define SWERVE_MODULE_H

#include <Arduino.h>
#include "stepper_motor.h"
#include "dc_motor.h"
#include "encoder.h"

/**
 * @brief Represents a single Swerve Drive Module containing:
 *  - 1x StepperMotor (DRV8825) for steering azimuth control
 *  - 1x DCMotor (L298N) for wheel propulsion drive
 *  - 1x EncoderReader (JGB37-520) for speed feedback tracking
 */
class SwerveModule {
private:
    uint8_t _id;
    StepperMotor _stepper;
    DCMotor _dcMotor;
    EncoderReader _encoder;

    float _currentAngleDegrees;
    float _targetAngleDegrees;
    uint16_t _stepsPerRev;
    uint8_t _microstepping;

public:
    /**
     * @brief Construct a new Swerve Module
     * @param moduleId Numeric identifier (1 to 4)
     * @param stepPin DRV8825 Step
     * @param dirPin DRV8825 Dir
     * @param enPin DRV8825 Enable
     * @param pwmPin L298N ENA/ENB PWM
     * @param in1Pin L298N IN1/IN3
     * @param in2Pin L298N IN2/IN4
     * @param encAPin Encoder Interrupt A
     * @param encBPin Encoder Direction B
     * @param stepsPerRev Stepper motor base steps per rev (default 200)
     * @param microstepping DRV8825 microstep division (default 1)
     */
    SwerveModule(uint8_t moduleId,
                 uint8_t stepPin, uint8_t dirPin, int8_t enPin,
                 int8_t pwmPin, uint8_t in1Pin, uint8_t in2Pin,
                 uint8_t encAPin, uint8_t encBPin,
                 uint16_t stepsPerRev = 200, uint8_t microstepping = 1);

    /**
     * @brief Initialize motor pins and encoders
     * @param isrFunc Function pointer to the encoder interrupt routine for this module
     */
    void begin(void (*isrFunc)());

    /**
     * @brief Must be called in main loop continuously to run stepper steps
     */
    void update();

    /**
     * @brief Handle encoder interrupt signal
     */
    void handleEncoderInterrupt();

    /**
     * @brief Set drive motor PWM speed (-255 to +255)
     */
    void setDriveSpeed(int speed);

    /**
     * @brief Set steering angle in degrees relative to current position
     * @param degrees Target relative angle (+ degrees CW, - degrees CCW)
     * @param rpm Steering rotation speed in RPM
     */
    void rotateSteering(float degrees, float rpm = 30.0f);

    /**
     * @brief Direct step pulse movement for steering calibration
     */
    void moveSteeringSteps(long steps, uint32_t stepDelayUs = 800);

    /**
     * @brief Stop all motion on this module (DC & Stepper)
     */
    void stop();

    /**
     * @brief Getters for module telemetry
     */
    uint8_t getId() const { return _id; }
    int getDriveSpeed() const { return _dcMotor.getSpeed(); }
    long getEncoderTicks() const { return _encoder.getTicks(); }
    float getDriveRPM(uint16_t pulsesPerRev = 330) { return _encoder.calculateRPM(pulsesPerRev); }
    bool isSteeringMoving() const { return _stepper.isMoving(); }
    void resetEncoder() { _encoder.resetTicks(); }
};

#endif // SWERVE_MODULE_H
