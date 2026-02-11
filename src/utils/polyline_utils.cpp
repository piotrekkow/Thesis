#include "polyline_utils.h"

#include <cmath>

#include "line.h"
#include "vector2.h"

namespace utils {
namespace {
inline double dot(const Vector2& a, const Vector2& b) {
    return a.dx * b.dx + a.dy * b.dy;
}

inline double length(const Vector2& v) { return std::sqrt(dot(v, v)); }

bool pointOnSegment(Position p, Position a, Position b, double eps = 1e-6) {
    Vector2 ap = p - a;
    Vector2 ab = b - a;

    double ab_len_sq = dot(ab, ab);
    double proj = dot(ap, ab);

    if (proj < -eps || proj > ab_len_sq + eps) return false;

    // distance from line
    double cross = ap.dx * ab.dy - ap.dy * ab.dx;
    return std::abs(cross) <= eps * std::sqrt(ab_len_sq);
}

bool segmentIntersection(Position a1, Position a2, Position b1, Position b2,
                         Position& out) {
    Line la(a1, a2);
    Line lb(b1, b2);

    if (!la.intersection(lb, out)) return false;

    return pointOnSegment(out, a1, a2) && pointOnSegment(out, b1, b2);
}

double segmentLength(Position a, Position b) { return length(b - a); }
}  // namespace

std::optional<PolylineIntersection> firstIntersection(const Polyline& A,
                                                      const Polyline& B) {
    PolylineIntersection result;

    const auto& pa = A.positions();
    const auto& pb = B.positions();

    if (pa.size() < 2 || pb.size() < 2) return std::nullopt;

    double accumulatedA = 0.0;

    for (size_t i = 0; i + 1 < pa.size(); ++i) {
        Position a1 = pa[i];
        Position a2 = pa[i + 1];

        double accumulatedB = 0.0;

        for (size_t j = 0; j + 1 < pb.size(); ++j) {
            Position b1 = pb[j];
            Position b2 = pb[j + 1];

            Position intersection;

            if (segmentIntersection(a1, a2, b1, b2, intersection)) {
                // compute local distances inside segments
                double distA = accumulatedA + segmentLength(a1, intersection);
                double distB = accumulatedB + segmentLength(b1, intersection);

                result.point = intersection;
                result.distanceA = distA;
                result.distanceB = distB;
                return result;  // first hit along A
            }

            accumulatedB += segmentLength(b1, b2);
        }

        accumulatedA += segmentLength(a1, a2);
    }

    return std::nullopt;
}

}  // namespace utils
