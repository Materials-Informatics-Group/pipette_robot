#pragma once

#include "WString.h"
#include "xy_system.h"
#include "lift.h"
#include "syringe_system.h"
#include "command.h"

// Top-level mode of operation (single active mode at a time)
enum class WorkingType {
    Moving,     // Continuous motion (XY/Lift) driven by update()
    Pipetting,  // Syringe ticks driven by update()
    Halting,    // Idle / stopped (safe state)
};

// Current movement direction while in Moving mode
enum class MovingDirection {
    None,
    // XYZ with positive or negative direction
    Xp,
    Xn,
    Yp,
    Yn,
    Zp,
    Zn,
};

// Internal controller state used by update()
struct RobotState {
    WorkingType type;    // Current mode
    MovingDirection dir; // Current direction (only meaningful in Moving)
};

// Robot coordinates subsystems and exposes a simple state machine:
// - fetch() receives a parsed Command and updates state
// - update() executes a small incremental motion every call
class Robot {
public:
    // xy_um_per_move   : XY travel per update() call (µm)
    // lift_um_per_move : Lift travel per update() call (µm)
    Robot(XYSystem& xy_system,
          Lift& lift,
          SyringeSystem& syringe_system,
          long xy_um_per_move,
          long lift_um_per_move);

    // Periodic step (called continuously from loop())
    void update();

    // XY motion primitives (one incremental move)
    void moveArmUp();
    void moveArmDown();
    void moveArmRight();
    void moveArmLeft();

    // Lift motion primitives (one incremental move)
    void moveLiftTop();
    void moveLiftBottom();

    // Queue syringe motion (ticks = discrete volume units)
    bool requestPullSyringes(int ticks);
    bool requestPushSyringes(int ticks);

    // Current syringe position in ticks
    int getSyringeCurrentPos();

    // Consume a command and return a log string (empty if ignored)
    String fetch(Command cmd);

private:
    // Subsystem references (owned outside)
    XYSystem& xy_system_;
    Lift& lift_;
    SyringeSystem& syringe_system_;

    // Motion granularity per update() call (µm)
    long xy_um_per_move_;
    long lift_um_per_move_;

    // Current controller state
    RobotState state_;
};
