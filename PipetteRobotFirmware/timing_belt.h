#pragma once

#include "stepper_motor.h"
#include "stdint.h"

// TimingBelt converts linear distance (µm) into motor steps.
// It abstracts the mechanical transmission (pulley + belt)
// so upper layers can operate in micrometers instead of steps.
class TimingBelt {
public:
    // motor : underlying stepper motor driver
    TimingBelt(StepperMotor& motor);

    // Move linear distance in micrometers (signed)
    // Positive/negative sign determines direction
    void move(long um);

private:
    StepperMotor& motor_;

    // Mechanical resolution:
    // Linear distance per one motor step (µm/step)
    // This value depends on pulley diameter and step angle.
    static constexpr long um_per_step_ = 200;

    // Convert micrometers to motor steps
    int umToStep(long um);
};
