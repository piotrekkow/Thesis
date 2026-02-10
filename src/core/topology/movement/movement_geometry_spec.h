#pragma once

#include <variant>

namespace topology {

struct LineParams {};

struct QuadBezierParams {};

struct CubicBezierParams {
    double control1Distance;
    double control2Distance;
};

using MovementGeometryParams =
    std::variant<LineParams, QuadBezierParams, CubicBezierParams>;

enum class MovementGeometryType { LINE, QUAD_BEZIER, CUBIC_BEZIER };

struct MovementGeometrySpec {
    MovementGeometryType type;

    double entryOffset = 0.0;
    double exitOffset = 0.0;

    MovementGeometryParams params;

    static MovementGeometrySpec line(double entryOffset = 4.0,
                                     double exitOffset = 4.0) {
        return MovementGeometrySpec{MovementGeometryType::LINE, entryOffset,
                                    exitOffset, LineParams{}};
    }

    static MovementGeometrySpec quadraticBezier(double entryOffset = 0.0,
                                                double exitOffset = 0.0) {
        return MovementGeometrySpec{MovementGeometryType::QUAD_BEZIER,
                                    entryOffset, exitOffset,
                                    QuadBezierParams{}};
    }

    static MovementGeometrySpec cubicBezier(double entryOffset = 0.0,
                                            double exitOffset = 0.0,
                                            double control1Distance = 10.0,
                                            double control2Distance = 10.0) {
        return MovementGeometrySpec{
            MovementGeometryType::CUBIC_BEZIER, entryOffset, exitOffset,
            CubicBezierParams{control1Distance, control2Distance}};
    }
};

}  // namespace topology
