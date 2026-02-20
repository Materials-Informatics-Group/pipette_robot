#include "stepper_motor.h"
#include <Arduino.h>
#include <math.h>

// Initialize pins and store pulse timing
StepperMotor::StepperMotor(int step_pin,
                           int dir_pin,
                           int pulse_width_us)
    : step_pin_(step_pin),
      dir_pin_(dir_pin),
      pulse_width_us_(pulse_width_us)
{
    // Configure control pins as outputs
    pinMode(step_pin, OUTPUT);
    pinMode(dir_pin, OUTPUT);
}

// Update pulse width (affects stepping speed)
void StepperMotor::setPulseWidth(int us) {
    pulse_width_us_ = us;
}

// Generate n step pulses
void StepperMotor::moveSteps(long n) {
    if (n == 0) return;

    // Set rotation direction
    if (n > 0) {
        digitalWrite(dir_pin_, HIGH);  // CW
    }
    else {
        digitalWrite(dir_pin_, LOW);   // CCW
        n = abs(n);                    // Convert to positive step count
    }

    // Generate STEP pulses
    // One step = HIGH → delay → LOW → delay
    for (int i = 0; i < n; i++) {
        digitalWrite(step_pin_, HIGH);
        delayMicroseconds(pulse_width_us_);
        digitalWrite(step_pin_, LOW);
        delayMicroseconds(pulse_width_us_);
    }
}
