#include "position.h"

#include "vector2.h"

namespace utils {

Position operator+(Position p, Vector2 v) { return {p.x + v.dx, p.y + v.dy}; }

Position operator-(Position p, Vector2 v) { return {p.x - v.dx, p.y - v.dy}; }

Vector2 operator-(Position a, Position b) { return {a.x - b.x, a.y - b.y}; }

bool operator==(Position a, Position b) { return a.x == b.x && a.y == b.y; }

bool operator!=(Position a, Position b) { return !(a == b); }

}  // namespace utils
