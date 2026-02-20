#pragma once

#include "stepper_motor.h"

// LeadScrew converts linear motion (µm) into motor steps
// for a lead screw mechanism.
// Compared to TimingBelt, this typically provides finer resolution.
class LeadScrew {
public:
    // motor : underlying stepper motor driver
    LeadScrew(StepperMotor& motor);

    // Move linear distance in micrometers (signed)
    // Positive/negative sign determines direction
    void move(long um);

private:
    StepperMotor& motor_;

    // Mechanical resolution:
    // Linear displacement per one motor step (µm/step)
    // Determined by lead screw pitch and motor step angle.
    static constexpr long um_per_step_ = 10;

    // Convert micrometers to motor steps
    int umToStep(long um);
};
