#pragma once

#include "lead_screw.h"
#include "direction.h"

// Lift controls vertical (Z-axis) motion using a lead screw.
// It operates in micrometers (µm) and applies axis direction correction
// before delegating movement to the mechanical layer.
class Lift {
public:
    // lead_screw : mechanical transmission for Z-axis
    // z_dir      : logical-to-physical direction mapping
    Lift(LeadScrew& lead_screw, AxisDirection z_dir);

    // Move in logical +Z direction (distance in µm)
    void moveTop(long um);

    // Move in logical -Z direction (distance in µm)
    void moveBottom(long um);

private:
    LeadScrew& lead_screw_;

    // Used to compensate for wiring/mechanical inversion
    AxisDirection z_dir_;
};
