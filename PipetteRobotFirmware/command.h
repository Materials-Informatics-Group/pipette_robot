#pragma once

#include <Arduino.h>

// High-level command categories received from serial input
enum class CommandType {
    Move,        // Continuous axis motion (X/Y/Z)
    Pipette,     // Syringe operation (pull/push)
    HaltMove,    // Stop current movement only
    HaltRobot,   // Emergency stop / fallback
};

// Axis movement directives
enum class MoveDirective {
    Xp,  // +X
    Xn,  // -X
    Yp,  // +Y
    Yn,  // -Y
    Zp,  // +Z
    Zn,  // -Z
};

// Pipette motion direction
enum class PipetteDirection {
    Pull, // Aspirate
    Push, // Dispense
};

// Pipette command payload
// value represents number of ticks (converted upstream from ml)
struct PipetteDirective {
    PipetteDirection dir;
    int value;
};

// Unified command structure parsed from serial string.
// Uses a union since Move and Pipette are mutually exclusive.
struct Command {
    CommandType type;
    union {
        MoveDirective move;     // Used when type == Move
        PipetteDirective pip;   // Used when type == Pipette
    };
};

// Parse a single-line serial command into a structured Command.
Command commandFromStr(const String& line);
