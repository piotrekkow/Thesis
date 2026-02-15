#pragma once

#include <optional>
#include <utility>

#include "position.h"

namespace utils {
class Polyline;
class Line;

struct PolylineIntersection {
    Position point;
    double distanceA;
    double distanceB;
};

std::optional<PolylineIntersection> firstIntersection(const Polyline& A,
                                                      const Polyline& B);

std::optional<std::pair<PolylineIntersection, PolylineIntersection>>
firstAndLastIntersection(const Polyline& A, const Line& B);
}  // namespace utils