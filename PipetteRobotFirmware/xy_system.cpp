#include "xy_system.h"
#include "direction.h"
#include "timing_belt.h"

// Store references and axis configuration
XYSystem::XYSystem(TimingBelt& x_belt,
                   TimingBelt& y_belt,
                   AxisDirection x_dir,
                   AxisDirection y_dir)
    : x_belt_(x_belt),
      y_belt_(y_belt),
      x_dir_(x_dir),
      y_dir_(y_dir) {}

// Move in +Y logical direction
// Apply direction correction before passing to belt
void XYSystem::moveUp(long um) {
    um = static_cast<long>(y_dir_) * um;
    y_belt_.move(um);
}

// Move in -Y logical direction
void XYSystem::moveDown(long um) {
    um = -static_cast<long>(y_dir_) * um;
    y_belt_.move(um);
}

// Move in +X logical direction
void XYSystem::moveRight(long um) {
    um = static_cast<long>(x_dir_) * um;
    x_belt_.move(um);
}

// Move in -X logical direction
void XYSystem::moveLeft(long um) {
    um = -static_cast<long>(x_dir_) * um;
    x_belt_.move(um);
}
