#include "robot.h"
#include "command.h"
#include <Arduino.h>

// Arduino sketch entry point.
// Receives text commands over Serial, updates robot state via fetch(),
// and performs incremental motion via robot.update() in the main loop.

// Step/dir pin assignments for each axis driver
#define STEP_PIN_X 2
#define DIR_PIN_X 5

#define STEP_PIN_Y 3
#define DIR_PIN_Y 6

#define STEP_PIN_Z 4
#define DIR_PIN_Z 7

#define STEP_PIN_A 12
#define DIR_PIN_A 13

// Enable pin for motor drivers (active level depends on the driver)
#define EN_PIN 8

// Initialize stepper motors (step pin, dir pin, pulse width in microseconds)
// pulse width: interval between HIGH and LOW on STEP pin (µs)
StepperMotor motor_x(STEP_PIN_X, DIR_PIN_X, 1000); // 1000 µs between HIGH and LOW pulses
StepperMotor motor_y(STEP_PIN_Y, DIR_PIN_Y, 1000);
StepperMotor motor_z(STEP_PIN_Z, DIR_PIN_Z, 1000);
StepperMotor motor_a(STEP_PIN_A, DIR_PIN_A, 1000);

// Map motors to mechanical components (linear motion abstractions)
TimingBelt belt_x(motor_x);
TimingBelt belt_y(motor_y);
LeadScrew lead_screw_lift(motor_z);
LeadScrew lead_screw_syringe(motor_a);

// Configure axis direction (to match wiring/mechanical orientation)
AxisDirection x_dir = AxisDirection::Reversed;
AxisDirection y_dir = AxisDirection::Reversed;
AxisDirection z_dir = AxisDirection::Normal;

// Build subsystems (operate in µm / ticks rather than raw steps)
XYSystem xy_system(belt_x, belt_y, x_dir, y_dir);
Lift lift(lead_screw_lift, z_dir);
SyringeSystem syringes(lead_screw_syringe, z_dir);

// Assemble the robot controller
// 10000 and 1000 are distances moved per robot.update() call (µm)
Robot robot(xy_system, lift, syringes, 10000, 1000); // XY: 10000 µm/update, Lift: 1000 µm/update

void setup() {
  // Start serial for command input/output (from the host/server)
  Serial.begin(9600);

  // Enable motor drivers
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);
}

void loop() {
  // Read and process one-line serial commands
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    if (line != "") {
      // Parse text command into structured command
      Command cmd = commandFromStr(line);

      // Update robot state machine and get optional log message
      String fetched_command = robot.fetch(cmd);

      // Reply back over serial (used by the server/UI for logging)
      if (fetched_command != String("")) {
        Serial.println(fetched_command);
      }
    }
  }

  // Execute one incremental motion step depending on current robot state
  robot.update();
}
