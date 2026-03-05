#pragma once

#include <unordered_map>

#include "conflict/map.h"
#include "id.h"

namespace geometry {
class Movement;
class Crossing;
}  // namespace geometry

namespace topology {
class MovementStructure;
}

namespace conflict {
class MapFactory {
   public:
    static Map compute(
        const std::unordered_map<MovementId, geometry::Movement>& movementGeos,
        const std::unordered_map<CrossingId, geometry::Crossing>& crossingGeos,
        const topology::MovementStructure& mStructure);
};
}  // namespace conflict