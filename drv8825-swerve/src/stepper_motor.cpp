#include "stepper_motor.h"

StepperMotor::StepperMotor(uint8_t stepPin, uint8_t dirPin, int8_t enablePin)
    : _stepPin(stepPin), _dirPin(dirPin), _enablePin(enablePin),
      _targetSteps(0), _stepIntervalUs(1000), _lastStepTimeUs(0),
      _stepState(false), _direction(true), _enabled(false) {
    _hasEnablePin = (enablePin >= 0);
}

void StepperMotor::begin() {
    pinMode(_stepPin, OUTPUT);
    pinMode(_dirPin, OUTPUT);
    digitalWrite(_stepPin, LOW);
    digitalWrite(_dirPin, HIGH);

    if (_hasEnablePin) {
        pinMode(_enablePin, OUTPUT);
        disable(); // Disable outputs on startup for safety
    }
}

void StepperMotor::setDirection(bool forward) {
    _direction = forward;
    digitalWrite(_dirPin, _direction ? HIGH : LOW);
}

void StepperMotor::enable() {
    if (_hasEnablePin) {
        digitalWrite(_enablePin, LOW); // DRV8825 EN is active LOW
    }
    _enabled = true;
}

void StepperMotor::disable() {
    if (_hasEnablePin) {
        digitalWrite(_enablePin, HIGH); // DRV8825 EN HIGH disables H-bridge
    }
    _enabled = false;
}

void StepperMotor::move(long steps, uint32_t stepIntervalUs) {
    if (steps == 0) return;

    if (steps > 0) {
        setDirection(true);
        _targetSteps = steps;
    } else {
        setDirection(false);
        _targetSteps = -steps;
    }

    _stepIntervalUs = (stepIntervalUs < 10) ? 10 : stepIntervalUs;
    _lastStepTimeUs = micros();
    enable();
}

void StepperMotor::moveRpm(long steps, float rpm, uint16_t stepsPerRev, uint8_t microstepping) {
    if (rpm <= 0.0f) return;
    
    // Total microsteps per revolution
    float totalMicrostepsPerRev = (float)stepsPerRev * (float)microstepping;
    // Seconds per step = 60 / (RPM * totalMicrostepsPerRev)
    // Microseconds per step:
    float intervalUs = (60.0f * 1000000.0f) / (rpm * totalMicrostepsPerRev);

    move(steps, (uint32_t)intervalUs);
}

bool StepperMotor::run() {
    if (_targetSteps <= 0) {
        return false;
    }

    uint32_t nowUs = micros();
    if (nowUs - _lastStepTimeUs >= _stepIntervalUs) {
        _lastStepTimeUs = nowUs;

        // Toggle STEP pin high for minimum pulse width
        digitalWrite(_stepPin, HIGH);
        delayMicroseconds(2); // DRV8825 requires min 1.9us step pulse width
        digitalWrite(_stepPin, LOW);

        _targetSteps--;

        if (_targetSteps == 0) {
            // Motion complete
            // Option: disable motor or keep enabled for holding torque
        }
        return true;
    }

    return false;
}

void StepperMotor::stop() {
    _targetSteps = 0;
}
