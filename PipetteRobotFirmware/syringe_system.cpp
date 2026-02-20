#include "syringe_system.h"
#include "direction.h"
#include "lead_screw.h"

// Initialize syringe state
SyringeSystem::SyringeSystem(LeadScrew& lead_screw, AxisDirection z_dir)
    : lead_screw_(lead_screw),
      z_dir_(z_dir)
{
    current_pos_ = 0;
    dir_ = SyringeDirection::None;
    remaining_ticks_ = 0;
}

// Validate and queue tick request
bool SyringeSystem::requestTicks(SyringeDirection dir, int ticks) {
    if (dir == SyringeDirection::None) {
        return false;
    }

    bool accepted = false;

    if (dir == SyringeDirection::Pull) {
        // Ensure we do not exceed capacity
        accepted = current_pos_ + ticks <= capacity_;
        if (accepted) {
            dir_ = SyringeDirection::Pull;
        }
    } else {
        // Ensure we do not push beyond zero
        accepted = current_pos_ >= ticks;
        if (accepted) {
            dir_ = SyringeDirection::Push;
        }
    }

    if (accepted) {
        remaining_ticks_ = ticks;
    }

    return accepted;
}

// Push all currently aspirated volume
void SyringeSystem::requestPushAll() {
    requestTicks(SyringeDirection::Push, current_pos_);
}

// Execute exactly one tick of plunger motion
void SyringeSystem::advanceOneTick() {
    if (dir_ == SyringeDirection::None) return;

    // If finished, stop motion
    if (remaining_ticks_ == 0) {
        if (dir_ == SyringeDirection::Pull) {
            // Apply slight correction after pull
            adjustPosition();
        }
        dir_ = SyringeDirection::None;
        return;
    }
    else if (remaining_ticks_ < 0) {
        // Safety fallback
        dir_ = SyringeDirection::None;
        remaining_ticks_ = 0;
        return;
    }

    int sign = 0;

    // Update logical position
    if (dir_ == SyringeDirection::Pull) {
        sign = 1;
        current_pos_++;
    } else {
        sign = -1;
        current_pos_--;
    }

    // Convert one tick into linear displacement (µm)
    long um = sign * static_cast<long>(z_dir_) * um_per_tick_;

    // Move plunger
    lead_screw_.move(um);

    remaining_ticks_--;
}

// Return current motion state
SyringeDirection SyringeSystem::getSyringeDirection() {
    return dir_;
}

// Return current plunger position in ticks
int SyringeSystem::getCurrentPos() {
    return current_pos_;
}

// Apply small forward/backward motion to reduce backlash
void SyringeSystem::adjustPosition() {
    long um = static_cast<long>(z_dir_) * um_per_tick_;
    lead_screw_.move(um);
    lead_screw_.move(-um);
}
