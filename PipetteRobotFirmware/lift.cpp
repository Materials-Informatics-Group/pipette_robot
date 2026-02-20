#include "lift.h"
#include "direction.h"
#include "lead_screw.h"

// Store mechanical reference and axis configuration
Lift::Lift(LeadScrew& lead_screw, AxisDirection z_dir)
    : lead_screw_(lead_screw),
      z_dir_(z_dir) {}

// Move in logical +Z direction (Top)
// Apply direction correction before passing to lead screw
void Lift::moveTop(long um) {
    um = static_cast<long>(z_dir_) * um;
    lead_screw_.move(um);
}

// Move in logical -Z direction (Bottom)
void Lift::moveBottom(long um) {
    um = -static_cast<long>(z_dir_) * um;
    lead_screw_.move(um);
}
