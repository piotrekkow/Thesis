#pragma once

#include "crossing_collisions.h"
#include "id.h"

namespace topology {
class MovementStructure;
}

namespace geometry {
class MovementMap;
class Crossing;
}  // namespace geometry

class CrossingCollisionMap {
   public:
    static CrossingCollisionMap build(
        const geometry::MovementMap& geoMovementMap,
        const std::unordered_map<CrossingId, geometry::Crossing>&
            geoCrossingMap);

    const std::unordered_map<CrossingId, CrossingCollisions>& map() const {
        return crossingCollisions_;
    }
    // const CrossingCollisions* tryFind(CrossingId crossingId) const;

   private:
    std::unordered_map<CrossingId, CrossingCollisions> crossingCollisions_;
};