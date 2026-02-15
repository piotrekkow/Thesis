#pragma once

#include "crossing_collisions.h"
#include "id.h"
#include "pair_hash.h"

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

    const std::unordered_map<std::pair<CrossingId, MovementId>,
                             CrossingCollisions, utils::PairHash>&
    crossingClearingMap() const {
        return crossingClearingMap_;
    }

    const std::unordered_map<std::pair<MovementId, CrossingId>,
                             CrossingCollisions, utils::PairHash>&
    movementClearingMap() const {
        return movementClearingMap_;
    }

   private:
    std::unordered_map<std::pair<CrossingId, MovementId>, CrossingCollisions,
                       utils::PairHash>
        crossingClearingMap_;

    std::unordered_map<std::pair<MovementId, CrossingId>, CrossingCollisions,
                       utils::PairHash>
        movementClearingMap_;
};