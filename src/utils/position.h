#pragma once

namespace geom {

class Vector2;

struct Position {
    double x;
    double y;
};

Position operator+(Position p, Vector2 v);
Position operator-(Position p, Vector2 v);
Vector2 operator-(Position a, Position b);

}  // namespace geom
