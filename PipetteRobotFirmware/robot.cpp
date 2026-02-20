#include <Arduino.h>
#include "robot.h"
#include "command.h"
#include "syringe_system.h"

// Construct robot controller and start in Halting state
Robot::Robot(XYSystem& xy_system, Lift& lift, SyringeSystem& syringe_system,
             long xy_um_per_move, long lift_um_per_move) :
            xy_system_(xy_system), lift_(lift), syringe_system_(syringe_system),
            xy_um_per_move_(xy_um_per_move), lift_um_per_move_(lift_um_per_move)
{
    // Default safe state (no motion)
    state_.type = WorkingType::Halting;
    state_.dir = MovingDirection::None;
}

void Robot::update() {
    // update() executes the current state machine action incrementally
    if (state_.type == WorkingType::Halting) return;

    if (state_.type == WorkingType::Moving) {
        // Continuous move: repeat one small step every update() call
        if (state_.dir == MovingDirection::None) return;
        else if (state_.dir == MovingDirection::Xp) {
                moveArmRight();
        }
        else if (state_.dir == MovingDirection::Xn) {
                moveArmLeft();
        }
        else if (state_.dir == MovingDirection::Yp) {
                moveArmUp();
        }
        else if (state_.dir == MovingDirection::Yn) {
                moveArmDown();
        }
        else if (state_.dir == MovingDirection::Zp) {
                moveLiftTop();
        }
        else if (state_.dir == MovingDirection::Zn) {
                moveLiftBottom();
        }
    }
    else { // Pipetting
        // Pipetting: advance one tick per update() call until completed
        if (syringe_system_.getSyringeDirection() == SyringeDirection::None) {
            // Auto-stop when syringe system finishes its queued ticks
            state_.type = WorkingType::Halting;
            state_.dir = MovingDirection::None;
            return;
        }
        syringe_system_.advanceOneTick();
    }
}

// XY movement helpers (distance is µm per call)
void Robot::moveArmUp() {
    xy_system_.moveUp(xy_um_per_move_);
}
void Robot::moveArmDown() {
    xy_system_.moveDown(xy_um_per_move_);
}
void Robot::moveArmRight() {
    xy_system_.moveRight(xy_um_per_move_);
}
void Robot::moveArmLeft() {
    xy_system_.moveLeft(xy_um_per_move_);
}

// Lift movement helpers (distance is µm per call)
void Robot::moveLiftTop() {
    lift_.moveTop(lift_um_per_move_);
}
void Robot::moveLiftBottom() {
    lift_.moveBottom(lift_um_per_move_);
}

// Forward syringe requests to SyringeSystem
bool Robot::requestPullSyringes(int ticks) {
    return syringe_system_.requestTicks(SyringeDirection::Pull, ticks);
}
bool Robot::requestPushSyringes(int ticks) {
    return syringe_system_.requestTicks(SyringeDirection::Push, ticks);
}

// Query syringe position (ticks)
int Robot::getSyringeCurrentPos() {
    return syringe_system_.getCurrentPos();
}

String Robot::fetch(Command cmd) {
    // fetch() updates the state machine based on a single command.
    // New actions are accepted only when Halting, except halt commands.
    String fetched_command = "";

    if (cmd.type == CommandType::HaltRobot) {
        // Global stop (also used as fallback for unknown commands)
        state_.type = WorkingType::Halting;
        state_.dir = MovingDirection::None;
        fetched_command = "Halt Robot";
    }
    else if (cmd.type == CommandType::HaltMove) {
        // Stop continuous movement only
        if (state_.type == WorkingType::Moving) {
            fetched_command = "Halt Move";
            state_.type = WorkingType::Halting;
            state_.dir = MovingDirection::None;
        }
    }
    else if (cmd.type == CommandType::Move) {
        // Start continuous movement only when idle
        if (state_.type == WorkingType::Halting) {
            state_.type = WorkingType::Moving;
            fetched_command = "Move ";

            // Map MoveDirective to internal MovingDirection
            if (cmd.move == MoveDirective::Xp) {
                state_.dir = MovingDirection::Xp;
                fetched_command += "X+";
            }
            else if (cmd.move == MoveDirective::Xn) {
                state_.dir = MovingDirection::Xn;
                fetched_command += "X-";
            }
            else if (cmd.move == MoveDirective::Yp) {
                state_.dir = MovingDirection::Yp;
                fetched_command += "Y+";
            }
            else if (cmd.move == MoveDirective::Yn) {
                state_.dir = MovingDirection::Yn;
                fetched_command += "Y-";
            }
            else if (cmd.move == MoveDirective::Zp) {
                state_.dir = MovingDirection::Zp;
                fetched_command += "Z+";
            }
            else if (cmd.move == MoveDirective::Zn) {
                state_.dir = MovingDirection::Zn;
                fetched_command += "Z-";
            }
        }
    }
    else if (cmd.type == CommandType::Pipette) {
        // Start pipetting only when idle
        if (state_.type == WorkingType::Halting) {
            state_.type = WorkingType::Pipetting;
            state_.dir = MovingDirection::None;

            // ticks represent discrete volume units (minimum_ml per tick)
            int ticks = cmd.pip.value;

            if (cmd.pip.dir == PipetteDirection::Pull) {
                // Queue pull ticks
                syringe_system_.requestTicks(SyringeDirection::Pull, ticks);

                // Build log message in ml
                fetched_command = "Pull ";
                fetched_command += String(float(ticks) * minimum_ml, 1);
                fetched_command += " ml";
            }
            else {
                // Push: -1 is a special "push all" request
                if (ticks == -1) {
                    syringe_system_.requestPushAll();
                    fetched_command = "Push All";
                }
                else {
                    // Queue push ticks
                    syringe_system_.requestTicks(SyringeDirection::Push, ticks);

                    // Build log message in ml
                    fetched_command = "Push ";
                    fetched_command += String(float(ticks) * minimum_ml, 1);
                    fetched_command += " ml";
                }
            }
        }
    }

    // Empty string means "no message to send back"
    return fetched_command;
}
