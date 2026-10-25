#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>

/**
 * @brief Quadrature Encoder reader for JGB37-520 DC Gear Motor Hall Sensors
 */
class EncoderReader {
private:
    uint8_t _pinA;
    uint8_t _pinB;
    volatile long _encoderTicks;
    uint32_t _lastRpmTime;
    long _lastRpmTicks;

public:
    EncoderReader(uint8_t pinA, uint8_t pinB) 
        : _pinA(pinA), _pinB(pinB), _encoderTicks(0), _lastRpmTime(0), _lastRpmTicks(0) {}

    void begin(void (*isrFunc)()) {
        pinMode(_pinA, INPUT_PULLUP);
        pinMode(_pinB, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(_pinA), isrFunc, RISING);
        _lastRpmTime = millis();
    }

    // Called inside Interrupt Service Routine (ISR)
    void handleInterrupt() {
        if (digitalRead(_pinB) == HIGH) {
            _encoderTicks++;
        } else {
            _encoderTicks--;
        }
    }

    long getTicks() const {
        return _encoderTicks;
    }

    void resetTicks() {
        _encoderTicks = 0;
        _lastRpmTicks = 0;
        _lastRpmTime = millis();
    }

    // Calculate approximate RPM based on ticks per revolution (PPR)
    // JGB37-520 usually has 11 PPR on motor shaft * Gear Ratio (e.g. 1:30 = 330 PPR)
    float calculateRPM(uint16_t pulsesPerRev) {
        uint32_t now = millis();
        uint32_t elapsedMs = now - _lastRpmTime;
        if (elapsedMs < 50) return 0.0f; // Prevent division by tiny time intervals

        long currentTicks = _encoderTicks;
        long deltaTicks = currentTicks - _lastRpmTicks;

        _lastRpmTicks = currentTicks;
        _lastRpmTime = now;

        float revolutions = (float)deltaTicks / (float)pulsesPerRev;
        float rpm = (revolutions / ((float)elapsedMs / 60000.0f));
        return rpm;
    }
};

#endif // ENCODER_H
