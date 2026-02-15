#pragma once

#include <unordered_map>

#include "id.h"
#include "movement_collisions.h"
#include "pair_hash.h"

namespace topology {
class MovementStructure;
}

namespace geometry {
class MovementMap;
}

class CollisionMap {
   public:
    static CollisionMap build(const geometry::MovementMap& geometryMap,
                              const topology::MovementStructure& mStructure);

    const std::unordered_map<std::pair<MovementId, MovementId>,
                             MovementCollisions, utils::PairHash>&
    map() const {
        return movementCollisions_;
    }

   private:
    std::unordered_map<std::pair<MovementId, MovementId>, MovementCollisions,
                       utils::PairHash>
        movementCollisions_;
};
