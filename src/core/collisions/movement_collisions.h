#pragma once

#include <vector>

#include "collision_point.h"

class MovementCollisions {
   public:
    MovementCollisions() : collisionPoints_{} {}
    void addCollisionPoint(CollisionPoint point) {
        collisionPoints_.push_back(point);
    }
    const std::vector<CollisionPoint>& points() const {
        return collisionPoints_;
    }

   private:
    std::vector<CollisionPoint> collisionPoints_;
};