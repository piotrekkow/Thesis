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
    Vector2 rotate(double radians) const;

   private:
    constexpr static double eps = 1e-9;
};

// operators — free functions
Vector2 operator+(Vector2 a, Vector2 b);
Vector2 operator-(Vector2 a, Vector2 b);
Vector2 operator*(Vector2 v, double s);
Vector2 operator*(double s, Vector2 v);
Vector2 operator/(Vector2 v, double s);
Vector2 operator-(Vector2 v);

// @return angle in range (-pi, pi) from vector produced from heading1 to vector
// produced from heading2
double angleBetween(double heading1, double heading2);

}  // namespace utils
