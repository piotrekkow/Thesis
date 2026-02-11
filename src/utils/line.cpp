#include "line.h"

#include <cmath>

#include "vector2.h"

namespace utils {
namespace {
struct LineEquation {
    double a, b, c;  // ax + by + c = 0
};

LineEquation toEquation(const Line& line) {
    double x1 = line.p1().x;
    double y1 = line.p1().y;
    double x2 = line.p2().x;
    double y2 = line.p2().y;

    double a = y1 - y2;
    double b = x2 - x1;
    double c = x1 * y2 - x2 * y1;

    return {a, b, c};
}

}  // namespace

Line::Line(Position p1, Position p2) : p1_(p1), p2_(p2) {}
Line::Line(Position p1, double heading, double length) : p1_(p1) {
    p2_ = p1 + Vector2::fromAngle(heading) * length;
}

// Vector2 Line::direction() const { return p2_ - p1_; }

bool Line::intersection(const Line& other, Position& intersection) const {
    auto eq1 = toEquation(*this);
    auto eq2 = toEquation(other);

    double det = eq1.a * eq2.b - eq2.a * eq1.b;
    if (std::abs(det) < eps) return false;

    double x = (eq1.b * eq2.c - eq2.b * eq1.c) / det;
    double y = (eq1.c * eq2.a - eq2.c * eq1.a) / det;

    intersection = Position(x, y);
    return true;
}

}  // namespace utils
