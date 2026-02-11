#pragma once

#include <vector>

#include "position.h"

namespace utils {

class Polyline {
   public:
    Polyline() = default;
    explicit Polyline(std::vector<Position> positions);
    Polyline(Position p1, Position p2, Position c,
             double tolerance = 0.1);  // quadratic Bezier curve
    Polyline(Position p1, Position p2, Position c1, Position c2,
             double tolerance = 0.1);  // cubic Bezier curve

    void setPositions(std::vector<Position> positions);
    void addPosition(Position position);
    void addQuadraticBezier(Position p1, Position p2, Position c,
                            double tolerance = 0.05);
    void addCubicBezier(Position p1, Position p2, Position c1, Position c2,
                        double tolerance = 0.05);

    const std::vector<Position>& positions() const { return positions_; }

   private:
    std::vector<Position> positions_;
};

}  // namespace utils
