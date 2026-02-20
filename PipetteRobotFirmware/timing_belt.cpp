#include "timing_belt.h"
#include "stepper_motor.h"
#include <Arduino.h>
#include "stdint.h"

// Store motor reference (no ownership)
TimingBelt::TimingBelt(StepperMotor& motor) : motor_(motor) {}

// Convert requested linear distance (µm) to steps
// and forward to the stepper motor
void TimingBelt::move(long um) {
    long n = umToStep(um);
    motor_.moveSteps(n);
}

// Simple integer conversion:
// steps = distance / (µm per step)
// Note: fractional steps are truncated.
int TimingBelt::umToStep(long um) {
    return um / um_per_step_;
}
