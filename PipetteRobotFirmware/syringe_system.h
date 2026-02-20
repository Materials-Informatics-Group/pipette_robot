#pragma once

#include <Arduino.h>
#include "direction.h"
#include "lead_screw.h"

// Geometric / volume constants
// axis_radius_cm : radius of syringe barrel (cm)
// minimum_ml     : volume represented by 1 tick (ml)
static constexpr float axis_radius_cm = 1.25 / 2;
static constexpr float minimum_ml = 0.2;

// Direction of syringe motion
enum class SyringeDirection {
    None,  // Idle
    Pull,  // Aspirate
    Push,  // Dispense
};

// SyringeSystem controls plunger motion via a lead screw.
// It converts discrete "ticks" (volume units) into linear motion (µm).
class SyringeSystem {
public:
    // lead_screw : mechanical Z actuator for plunger
    // z_dir      : direction correction (Normal/Reversed)
    SyringeSystem(LeadScrew& lead_screw, AxisDirection z_dir);

    // Request movement in discrete ticks.
    // ticks correspond to minimum_ml per tick.
    // Returns true if the request is within capacity limits.
    bool requestTicks(SyringeDirection dir, int ticks);

    // Convenience: push entire current volume.
    void requestPushAll();

    // Execute exactly one tick of motion.
    // Called repeatedly from Robot::update().
    void advanceOneTick();

    // Current active syringe direction
    SyringeDirection getSyringeDirection();

    // Current position in ticks (0 ... capacity_)
    int getCurrentPos();

    // Small corrective motion to compensate backlash/mechanical play
    void adjustPosition();

private:
    LeadScrew& lead_screw_;

    int current_pos_;       // Current plunger position (ticks)
    int remaining_ticks_;   // Remaining ticks to execute
    SyringeDirection dir_;  // Current motion direction
    AxisDirection z_dir_;   // Direction correction

    // Maximum capacity in ticks (e.g., 25 ticks × 0.2 ml = 5 ml)
    static constexpr int capacity_ = 25;

    // Linear displacement per tick (µm).
    // Derived from:
    //   1 tick = minimum_ml volume
    //   volume = area × displacement
    //   area = πr²
    // Converted to micrometers.
    static constexpr long um_per_tick_ =
        round((10 * 1000 / 5) / (axis_radius_cm * axis_radius_cm * PI));
};
