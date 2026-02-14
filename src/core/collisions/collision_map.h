#pragma once

#include <unordered_map>

#include "id.h"
#include "movement_collisions.h"

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

    const std::unordered_map<MovementId, MovementCollisions>& map() const {
        return movementCollisions_;
    }
    const MovementCollisions* tryFind(MovementId movementId) const;

   private:
    std::unordered_map<MovementId, MovementCollisions> movementCollisions_;
};
