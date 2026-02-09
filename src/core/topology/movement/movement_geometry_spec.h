#pragma once

#include <variant>

struct LineParams {
    double entryOffset;
    double exitOffset;
};

struct QuadBezierParams {
    double entryOffset;
    double exitOffset;
};

struct CubicBezierParams {
    double entryOffset;
    double exitOffset;
    double control1Distance;
    double control2Distance;
};

using MovementGeometryParams =
    std::variant<LineParams, QuadBezierParams, CubicBezierParams>;

enum class MovementGeometryType { LINE, QUAD_BEZIER, CUBIC_BEZIER };

struct MovementGeometrySpec {
    MovementGeometryType type;
    MovementGeometryParams params;

    static MovementGeometrySpec line(double entryOffset = 0.0,
                                     double exitOffset = 0.0) {
        return MovementGeometrySpec{MovementGeometryType::LINE,
                                    LineParams{entryOffset, exitOffset}};
    }

    static MovementGeometrySpec quadraticBezier(double entryOffset = 0.0,
                                                double exitOffset = 0.0) {
        return MovementGeometrySpec{MovementGeometryType::QUAD_BEZIER,
                                    QuadBezierParams{entryOffset, exitOffset}};
    }

    static MovementGeometrySpec cubicBezier(double entryOffset = 0.0,
                                            double exitOffset = 0.0,
                                            double control1Distance = 10.0,
                                            double control2Distance = 10.0) {
        return MovementGeometrySpec{
            MovementGeometryType::CUBIC_BEZIER,
            CubicBezierParams{entryOffset, exitOffset, control1Distance,
                              control2Distance}};
    }
};
