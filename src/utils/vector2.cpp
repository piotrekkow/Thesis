#include "vector2.h"

#include <cassert>
#include <cmath>

namespace utils {

Vector2 Vector2::fromAngle(double radians) {
    return {std::cos(radians), std::sin(radians)};
}

double Vector2::length() const { return std::sqrt(dx * dx + dy * dy); }

bool Vector2::isZero() const { return dx * dx + dy * dy < eps * eps; }

Vector2 Vector2::normalized() const {
    double len = length();
    assert(len > eps && "Cannot normalize zero vector");
    return {dx / len, dy / len};
}

double Vector2::dot(Vector2 v) const { return dx * v.dx + dy * v.dy; }

double Vector2::cross(Vector2 v) const { return dx * v.dy - dy * v.dx; }

double Vector2::angle() const {
    assert(!isZero() && "Cannot get angle of zero vector");
    return std::atan2(dy, dx);
}

double Vector2::angleTo(Vector2 v) const {
    assert(!isZero() && !v.isZero() && "Cannot get angle to zero vector");
    return std::atan2(cross(v), dot(v));
}

Vector2 operator+(Vector2 a, Vector2 b) { return {a.dx + b.dx, a.dy + b.dy}; }

Vector2 operator-(Vector2 a, Vector2 b) { return {a.dx - b.dx, a.dy - b.dy}; }

Vector2 operator*(Vector2 v, double s) { return {v.dx * s, v.dy * s}; }

Vector2 operator*(double s, Vector2 v) { return v * s; }

Vector2 operator/(Vector2 v, double s) { return {v.dx / s, v.dy / s}; }

Vector2 operator-(Vector2 v) { return {-v.dx, -v.dy}; }

}  // namespace utils
