#pragma once

#include <optional>

#include "position.h"

namespace conflict {
class Point {
   public:
    Point(double clearingDistance, double enteringDistance,
          utils::Position position,
          std::optional<double> clearingMaxK = std::nullopt)
        : clearingDistance_(clearingDistance),
          enteringDistance_(enteringDistance),
          position_(position),
          clearingMaxCurvature_(clearingMaxK) {}

    double clearingDistance() const { return clearingDistance_; }
    double enteringDistance() const { return enteringDistance_; }
    utils::Position position() const { return position_; }
    // clearing path max curvature
    std::optional<double> clearingMaxK() const { return clearingMaxCurvature_; }

   private:
    double clearingDistance_;
    double enteringDistance_;
    utils::Position position_;
    std::optional<double> clearingMaxCurvature_;
};
}  // namespace conflict