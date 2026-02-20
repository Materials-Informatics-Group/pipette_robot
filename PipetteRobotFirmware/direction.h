#pragma once

// AxisDirection defines logical-to-physical direction mapping.
// It is used to compensate for wiring or mechanical orientation differences.
//
// Normal   : logical positive → physical positive
// Reversed : logical positive → physical negative
//
// Internally stored as +1 or -1 so it can directly multiply distances.
enum class AxisDirection : int {
    Normal = +1,
    Reversed = -1,
};
