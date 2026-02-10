#include "polyline.h"

namespace utils {

namespace {

bool isQuadFlat(Position p1, Position p2, Position c, double tolerance) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;

    // Cross product to find distance from point 'c' to line 'p1-p2'
    double ux = c.x - p1.x;
    double uy = c.y - p1.y;
    double area = ux * dy - uy * dx;

    // Squared distance from c to line p1-p2
    double distSq = (area * area) / (dx * dx + dy * dy);

    return distSq <= (tolerance * tolerance);
}

bool isCubicFlat(Position p1, Position p2, Position c1, Position c2,
                 double tolerance) {
    double dx = p2.x - p1.x;
    double dy = p2.y - p1.y;
    double tolSq = tolerance * tolerance;
    double lineLenSq = dx * dx + dy * dy;

    // Handle the case where p1 and p2 are the same point (degenerated line)
    if (lineLenSq < 1e-9) {
        double d1sq =
            (c1.x - p1.x) * (c1.x - p1.x) + (c1.y - p1.y) * (c1.y - p1.y);
        double d2sq =
            (c2.x - p1.x) * (c2.x - p1.x) + (c2.y - p1.y) * (c2.y - p1.y);
        return d1sq <= tolSq && d2sq <= tolSq;
    }

    // Distance for c1
    double area1 = (c1.x - p1.x) * dy - (c1.y - p1.y) * dx;
    if ((area1 * area1) / lineLenSq > tolSq) return false;

    // Distance for c2
    double area2 = (c2.x - p1.x) * dy - (c2.y - p1.y) * dx;
    if ((area2 * area2) / lineLenSq > tolSq) return false;

    return true;
}

void subdivideQuadraticBezier(Position p1, Position p2, Position c,
                              double tolerance, std::vector<Position>& out) {
    Position m0 = {(p1.x + c.x) / 2.0, (p1.y + c.y) / 2.0};
    Position m1 = {(c.x + p2.x) / 2.0, (c.y + p2.y) / 2.0};
    Position m2 = {(m0.x + m1.x) / 2.0, (m0.y + m1.y) / 2.0};

    if (isQuadFlat(p1, p2, c, tolerance)) {
        out.push_back(p2);  // Segment is flat enough
    } else {
        subdivideQuadraticBezier(p1, m2, m0, tolerance, out);
        subdivideQuadraticBezier(m2, p2, m1, tolerance, out);
    }
}

void subdivideCubicBezier(Position p1, Position p2, Position c1, Position c2,
                          double tolerance, std::vector<Position>& out) {
    Position m0 = {(p1.x + c1.x) / 2, (p1.y + c1.y) / 2};
    Position m1 = {(c1.x + c2.x) / 2, (c1.y + c2.y) / 2};
    Position m2 = {(c2.x + p2.x) / 2, (c2.y + p2.y) / 2};
    Position q0 = {(m0.x + m1.x) / 2, (m0.y + m1.y) / 2};
    Position q1 = {(m1.x + m2.x) / 2, (m1.y + m2.y) / 2};
    Position b = {(q0.x + q1.x) / 2, (q0.y + q1.y) / 2};

    if (isCubicFlat(p1, p2, c1, c2, tolerance)) {
        out.push_back(p2);
    } else {
        subdivideCubicBezier(p1, b, m0, q0, tolerance, out);
        subdivideCubicBezier(b, p2, q1, m2, tolerance, out);
    }
}
}  // namespace

Polyline::Polyline(std::vector<Position> positions) : positions_(positions) {}

Polyline::Polyline(Position p1, Position p2, Position c, double tolerance) {
    positions_.push_back(p1);
    subdivideQuadraticBezier(p1, p2, c, tolerance, positions_);
}

Polyline::Polyline(Position p1, Position p2, Position c1, Position c2,
                   double tolerance) {
    positions_.push_back(p1);
    subdivideCubicBezier(p1, p2, c1, c2, tolerance, positions_);
}

void Polyline::addPosition(Position position) {
    positions_.push_back(position);
}
}  // namespace utils