#pragma once

#include "position.h"

namespace utils {

class Line {
   public:
    Line(Position p1, Position p2);
    Line(Position p1, double heading, double length = 1.0);

    void setP1(Position p1) { p1_ = p1; }
    void setP2(Position p2) { p2_ = p2; }

    Position p1() const { return p1_; }
    Position p2() const { return p2_; }

    // @return true if lines intersect, false otherwise
    // @param intersection output parameter for intersection point
    bool intersection(const Line& other, Position& intersection) const;

   private:
    Position p1_;
    Position p2_;

    static constexpr double eps = 1e-6;
};

}  // namespace utils