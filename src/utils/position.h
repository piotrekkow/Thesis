#pragma once

namespace utils {

class Vector2;

struct Position {
    double x;
    double y;
};

Position operator+(Position p, Vector2 v);
Position operator-(Position p, Vector2 v);
Vector2 operator-(Position a, Position b);

}  // namespace utils
