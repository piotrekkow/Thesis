#pragma once

#include "polyline_utils.h"
#include "position.h"
class CollisionPoint {
   public:
    CollisionPoint(double distanceEntering, double distanceClearing,
                   utils::Position position)
        : distanceClearing_(distanceClearing),
          distanceEntering_(distanceEntering),
          position_(position) {}

    explicit CollisionPoint(utils::PolylineIntersection& pi) {
        distanceClearing_ = pi.distanceA;
        distanceEntering_ = pi.distanceB;
        position_ = pi.point;
    }

    double distanceClearing() const { return distanceClearing_; }
    double distanceEntering() const { return distanceEntering_; }
    utils::Position position() const { return position_; }

   private:
    double distanceClearing_;
    double distanceEntering_;
    utils::Position position_;
};