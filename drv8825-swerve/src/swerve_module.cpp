#include "swerve_module.h"

SwerveModule::SwerveModule(uint8_t moduleId,
                           uint8_t stepPin, uint8_t dirPin, int8_t enPin,
                           int8_t pwmPin, uint8_t in1Pin, uint8_t in2Pin,
                           uint8_t encAPin, uint8_t encBPin,
                           uint16_t stepsPerRev, uint8_t microstepping)
    : _id(moduleId),
      _stepper(stepPin, dirPin, enPin),
      _dcMotor(pwmPin, in1Pin, in2Pin),
      _encoder(encAPin, encBPin),
      _currentAngleDegrees(0.0f),
      _targetAngleDegrees(0.0f),
      _stepsPerRev(stepsPerRev),
      _microstepping(microstepping) {}

void SwerveModule::begin(void (*isrFunc)()) {
    _stepper.begin();
    _dcMotor.begin();
    _encoder.begin(isrFunc);
}

void SwerveModule::update() {
    _stepper.run();
}

void SwerveModule::handleEncoderInterrupt() {
    _encoder.handleInterrupt();
}

void SwerveModule::setDriveSpeed(int speed) {
    _dcMotor.setSpeed(speed);
}

void SwerveModule::rotateSteering(float degrees, float rpm) {
    if (degrees == 0.0f) return;

    // Calculate total steps required for desired angular displacement
    // Total steps per full 360 rotation = stepsPerRev * microstepping
    float stepsPerDegree = ((float)_stepsPerRev * (float)_microstepping) / 360.0f;
    long targetSteps = (long)(degrees * stepsPerDegree);

    _stepper.moveRpm(targetSteps, rpm, _stepsPerRev, _microstepping);
    _currentAngleDegrees += degrees;
}

void SwerveModule::moveSteeringSteps(long steps, uint32_t stepDelayUs) {
    _stepper.move(steps, stepDelayUs);
}

void SwerveModule::stop() {
    _dcMotor.stop();
    _stepper.stop();
}
