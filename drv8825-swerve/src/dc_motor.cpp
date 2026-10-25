#include "dc_motor.h"

DCMotor::DCMotor(int8_t pwmPin, uint8_t in1Pin, uint8_t in2Pin)
    : _pwmPin(pwmPin), _in1Pin(in1Pin), _in2Pin(in2Pin), _currentSpeed(0) {}

void DCMotor::begin() {
    if (_pwmPin >= 0) {
        pinMode(_pwmPin, OUTPUT);
    }
    pinMode(_in1Pin, OUTPUT);
    pinMode(_in2Pin, OUTPUT);
    stop();
}

void DCMotor::setSpeed(int speed) {
    speed = constrain(speed, -255, 255);
    _currentSpeed = speed;

    if (speed > 0) {
        // Forward rotation
        digitalWrite(_in1Pin, HIGH);
        digitalWrite(_in2Pin, LOW);
        if (_pwmPin >= 0) {
            analogWrite(_pwmPin, speed);
        }
    } else if (speed < 0) {
        // Reverse rotation
        digitalWrite(_in1Pin, LOW);
        digitalWrite(_in2Pin, HIGH);
        if (_pwmPin >= 0) {
            analogWrite(_pwmPin, -speed);
        }
    } else {
        // Coasting stop
        stop();
    }
}

void DCMotor::stop() {
    _currentSpeed = 0;
    digitalWrite(_in1Pin, LOW);
    digitalWrite(_in2Pin, LOW);
    if (_pwmPin >= 0) {
        analogWrite(_pwmPin, 0);
    }
}

void DCMotor::brake() {
    _currentSpeed = 0;
    digitalWrite(_in1Pin, HIGH);
    digitalWrite(_in2Pin, HIGH);
    if (_pwmPin >= 0) {
        analogWrite(_pwmPin, 255);
    }
}
