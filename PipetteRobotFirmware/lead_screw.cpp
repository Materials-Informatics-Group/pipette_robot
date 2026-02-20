#include <Arduino.h>
#include "lead_screw.h"
#include "stepper_motor.h"

// Store motor reference (no ownership)
LeadScrew::LeadScrew(StepperMotor& motor) : motor_(motor) {}

// Convert requested linear displacement (µm) into motor steps
// and forward to the stepper motor
void LeadScrew::move(long um) {
    int n = umToStep(um);
    motor_.moveSteps(n);
}

// steps = distance / (µm per step)
// Note: fractional steps are truncated (integer division).
int LeadScrew::umToStep(long um) {
    return um / um_per_step_;
}
