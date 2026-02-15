#pragma once

#include <utility>
#include <vector>

#include "collision_point.h"

class CrossingCollisions {
   public:
    void addCollisionPointPair(CollisionPoint point1, CollisionPoint point2) {
        points_.push_back(std::make_pair(point1, point2));
    }
    void addCollisionPointPair(std::pair<CollisionPoint, CollisionPoint> pair) {
        points_.push_back(pair);
    }
    const std::vector<std::pair<CollisionPoint, CollisionPoint>>& points()
        const {
        return points_;
    }

   private:
    std::vector<std::pair<CollisionPoint, CollisionPoint>> points_;
};