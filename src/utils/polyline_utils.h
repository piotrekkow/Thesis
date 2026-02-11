#pragma once

#include <optional>

#include "polyline.h"
#include "position.h"

namespace utils {

struct PolylineIntersection {
    Position point;
    double distanceA;
    double distanceB;
};

std::optional<PolylineIntersection> firstIntersection(const Polyline& A,
                                                      const Polyline& B);

}  // namespace utils