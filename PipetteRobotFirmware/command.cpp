#include <Arduino.h>
#include <stdio.h>
#include "command.h"

// Convert a serial input string into a Command structure.
// Expected formats:
//
// Movement:
//   "X+"  "X-"  "Y+"  "Y-"  "Z+"  "Z-"
//
// Halt movement:
//   "RELEASED"
//
// Pipette:
//   "PULL <ticks>"
//   "PUSH <ticks>"
//
// Any unknown command defaults to HaltRobot.
Command commandFromStr(const String& line) {
    int p = line.indexOf(' ');
    Command cmd;

    // No space → simple movement or halt command
    if (p == -1) {
        const String& str = line;

        if (str == "X+") {
            cmd.type = CommandType::Move;
            cmd.move = MoveDirective::Xp;
        }
        else if (str == "X-") {
            cmd.type = CommandType::Move;
            cmd.move = MoveDirective::Xn;
        }
        else if (str == "Y+") {
            cmd.type = CommandType::Move;
            cmd.move = MoveDirective::Yp;
        }
        else if (str == "Y-") {
            cmd.type = CommandType::Move;
            cmd.move = MoveDirective::Yn;
        }
        else if (str == "Z+") {
            cmd.type = CommandType::Move;
            cmd.move = MoveDirective::Zp;
        }
        else if (str == "Z-") {
            cmd.type = CommandType::Move;
            cmd.move = MoveDirective::Zn;
        }
        else if (str == "RELEASED") {
            // Stop continuous movement
            cmd.type = CommandType::HaltMove;
        }
        else {
            // Unknown command → emergency halt
            cmd.type = CommandType::HaltRobot;
        }
    }
    else {
        // Space found → pipette command with argument
        const String& str = line.substring(0, p);
        int ticks = line.substring(p + 1).toInt();

        if (str == "PULL") {
            cmd.type = CommandType::Pipette;
            cmd.pip = {
                .dir = PipetteDirection::Pull,
                .value = ticks,
            };
        }
        else if (str == "PUSH") {
            cmd.type = CommandType::Pipette;
            cmd.pip = {
                .dir = PipetteDirection::Push,
                .value = ticks,
            };
        }
        else {
            // Unknown formatted command → emergency halt
            cmd.type = CommandType::HaltRobot;
        }
    }

    return cmd;
}
