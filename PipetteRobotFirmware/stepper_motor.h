#pragma once

// Low-level driver for a step/dir type stepper motor driver.
// This class directly generates STEP pulses with a configurable pulse width (µs).
class StepperMotor {
public:
    // step_pin       : GPIO connected to STEP input of the driver
    // dir_pin        : GPIO connected to DIR input of the driver
    // pulse_width_us : delay between HIGH and LOW (µs), controls stepping speed
    StepperMotor(int step_pin, int dir_pin, int pulse_width_us);

    // Change pulse width (µs)
    // Smaller value → faster stepping
    // Larger value  → slower stepping
    void setPulseWidth(int us);

    // Move motor by n steps
    // n > 0 : CW rotation (DIR = HIGH)
    // n < 0 : CCW rotation (DIR = LOW)
    void moveSteps(long n);

private:
    int dir_pin_;         // Direction control pin
    int step_pin_;        // Step pulse pin
    int pulse_width_us_;  // Interval between HIGH and LOW pulses (µs)
};
