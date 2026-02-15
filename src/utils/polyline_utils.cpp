#include "polyline_utils.h"

#include <cmath>

#include "line.h"
#include "polyline.h"
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

bool segmentIntersectionInclusive(Position a1, Position a2, Position b1,
                                  Position b2, Position& out) {
    Vector2 r = a2 - a1;
    Vector2 s = b2 - b1;

    double rxs = r.dx * s.dy - r.dy * s.dx;
    Vector2 qp = b1 - a1;
    double qpxr = qp.dx * r.dy - qp.dy * r.dx;

    const double eps = 1e-6;

    // CASE 1 — collinear
    if (std::abs(rxs) < eps && std::abs(qpxr) < eps) {
        double r_len_sq = dot(r, r);
        if (r_len_sq < eps) return false;

        // project B endpoints onto A
        double t0 = dot(b1 - a1, r) / r_len_sq;
        double t1 = dot(b2 - a1, r) / r_len_sq;

        if (t0 > t1) std::swap(t0, t1);

        // check overlap
        if (t1 < -eps || t0 > 1.0 + eps) return false;

        // first overlap along A
        double t = std::max(0.0, t0);
        out = a1 + r * t;
        return true;
    }

    // CASE 2 — parallel but not collinear
    if (std::abs(rxs) < eps) return false;

    // CASE 3 — proper intersection
    double t = (qp.dx * s.dy - qp.dy * s.dx) / rxs;
    double u = (qp.dx * r.dy - qp.dy * r.dx) / rxs;

    if (t >= -eps && t <= 1.0 + eps && u >= -eps && u <= 1.0 + eps) {
        out = a1 + r * t;
        return true;
    }

    return false;
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

            if (segmentIntersectionInclusive(a1, a2, b1, b2, intersection)) {
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

std::optional<std::pair<PolylineIntersection, PolylineIntersection>>
firstAndLastIntersection(const Polyline& A, const Line& B) {
    const auto& pa = A.positions();
    if (pa.size() < 2) return std::nullopt;

    Position b1 = B.p1();
    Position b2 = B.p2();

    bool foundAny = false;

    PolylineIntersection firstHit{};
    PolylineIntersection lastHit{};

    double accumulatedA = 0.0;

    for (size_t i = 0; i + 1 < pa.size(); ++i) {
        Position a1 = pa[i];
        Position a2 = pa[i + 1];

        Position intersection;

        if (segmentIntersectionInclusive(a1, a2, b1, b2, intersection)) {
            double distA = accumulatedA + segmentLength(a1, intersection);
            double distB = segmentLength(b1, intersection);

            PolylineIntersection current;
            current.point = intersection;
            current.distanceA = distA;
            current.distanceB = distB;

            if (!foundAny) {
                firstHit = current;
                lastHit = current;
                foundAny = true;
            } else {
                // update last hit along A
                if (distA > lastHit.distanceA) {
                    lastHit = current;
                }

                // defensive: if numerical noise produced earlier point
                if (distA < firstHit.distanceA) {
                    firstHit = current;
                }
            }
        }

        accumulatedA += segmentLength(a1, a2);
    }

    if (!foundAny) return std::nullopt;
    return std::make_pair(firstHit, lastHit);
}

}  // namespace utils
