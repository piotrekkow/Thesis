#pragma once

#include <unordered_map>

#include "id.h"
#include "movement_collisions.h"

namespace geometry {
class MovementMap;
}

class CollisionMap {
   public:
    static CollisionMap build(geometry::MovementMap& movementMap);
    const std::unordered_map<EdgeId, MovementCollisions>* tryFrom(
        EdgeId from) const;
    const MovementCollisions* tryFind(EdgeId from, EdgeId to) const;

    const std::unordered_map<EdgeId,
                             std::unordered_map<EdgeId, MovementCollisions>>&
    collisionMap() const {
        return collisionMap_;
    }

   private:
    // map of key: fromId (entering) edges and value: (map of key: toId
    // (exiting) edges and value: movement collisions)
    std::unordered_map<EdgeId, std::unordered_map<EdgeId, MovementCollisions>>
        collisionMap_;
};
