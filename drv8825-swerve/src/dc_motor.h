#ifndef DC_MOTOR_H
#define DC_MOTOR_H

#include <Arduino.h>

/**
 * @brief Class representing a single DC motor driven by L298N (or similar H-bridge).
 */
class DCMotor {
private:
    int8_t _pwmPin;
    uint8_t _in1Pin;
    uint8_t _in2Pin;
    int _currentSpeed;

public:
    /**
     * @brief Construct a new DCMotor object
     * @param pwmPin ENA/ENB pin on L298N (Set to -1 if jumper is attached for 100% ON/OFF speed)
     * @param in1Pin IN1/IN3 direction control pin
     * @param in2Pin IN2/IN4 direction control pin
     */
    DCMotor(int8_t pwmPin, uint8_t in1Pin, uint8_t in2Pin);

    /**
     * @brief Initialize GPIO pins and default state
     */
    void begin();

    /**
     * @brief Set motor speed and direction
     * @param speed Range from -255 (full reverse) to +255 (full forward). 0 stops motor.
     */
    void setSpeed(int speed);

    /**
     * @brief Coast motor to a stop (IN1=LOW, IN2=LOW, PWM=0)
     */
    void stop();

    /**
     * @brief Active electronic brake (IN1=HIGH, IN2=HIGH, PWM=255)
     */
    void brake();

    /**
     * @brief Get currently set speed
     * @return int Current speed (-255 to 255)
     */
    int getSpeed() const { return _currentSpeed; }
};

#endif // DC_MOTOR_H
