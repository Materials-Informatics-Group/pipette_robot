#pragma once

#include "timing_belt.h"
#include "direction.h"

// XYSystem provides 2D planar motion control using two timing belts.
// It operates in micrometers (µm) and applies axis direction correction
// before forwarding motion to the belt mechanisms.
class XYSystem {
public:
    // x_belt, y_belt : mechanical transmission layers
    // x_dir, y_dir   : direction correction for each axis
    XYSystem(TimingBelt& x_belt,
             TimingBelt& y_belt,
             AxisDirection x_dir,
             AxisDirection y_dir);

    // Move in logical directions (distance in µm)
    void moveUp(long um);
    void moveDown(long um);
    void moveRight(long um);
    void moveLeft(long um);

private:
    TimingBelt& x_belt_;
    TimingBelt& y_belt_;

    // Used to align logical coordinate system with hardware orientation
    AxisDirection x_dir_;
    AxisDirection y_dir_;
};
