#pragma once

namespace utils {

struct Vector2 {
    double dx;
    double dy;

    static Vector2 fromAngle(double radians);

    bool isZero() const;

    double length() const;
    Vector2 normalized() const;

    double dot(Vector2 v) const;
    double cross(Vector2 v) const;
    double angle() const;
    double angleTo(Vector2 v) const;

   private:
    constexpr static double eps = 1e-9;
};

// operators — free functions
Vector2 operator+(Vector2 a, Vector2 b);
Vector2 operator-(Vector2 a, Vector2 b);
Vector2 operator*(Vector2 v, double s);
Vector2 operator*(double s, Vector2 v);
Vector2 operator/(Vector2 v, double s);

}  // namespace utils
