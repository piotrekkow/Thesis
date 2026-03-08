#include "geometry/calculators/movement_factory.h"

#include <qdebug.h>
#include <qnamespace.h>

#include <utility>

#include "debug_sink.h"
#include "geometry/calculators/edge_factory.h"
#include "geometry/edge.h"
#include "geometry/movement.h"
#include "network.h"
#include "polyline.h"
#include "position.h"
#include "topology/edge.h"
#include "topology/lane_group.h"
#include "topology/movement/movement_geometry_spec.h"
#include "topology/movement/movement_structure.h"
#include "topology/node.h"
#include "utils/position.h"
#include "utils/vector2.h"

namespace geometry {
namespace {

// calculate midpoint anchor as base for movement mitre calculations
utils::Position offsetAnchor(utils::Line& leftmost, utils::Line& rightmost) {
    utils::Position p = leftmost.p1();

    if ((leftmost.p1() != rightmost.p1()) ||
        (leftmost.p2() != rightmost.p2())) {
        utils::Vector2 lateral = (rightmost.p1() - leftmost.p1()) / 2.0;
        p = p + lateral;
    }
    return p;
}

// pass entry/exit lines instead of positions to ensure correct cubic bezier
// direction which could not be ensured with just (offset - start) due to mitre
// calculations
utils::Polyline buildPath(utils::Line entry, utils::Position entryOffset,
                          utils::Position exitOffset, utils::Line exit,
                          const topology::MovementGeometrySpec& geometrySpec) {
    utils::Polyline path;
    path.addPosition(entry.p1());

    switch (geometrySpec.type) {
        default:
        case topology::MovementGeometryType::LINE: {
            path.addPosition(entryOffset);
            path.addPosition(exitOffset);
            break;
        }
        case topology::MovementGeometryType::QUAD_BEZIER: {
            utils::Line entryLine(entry.p1(), entryOffset);
            utils::Line exitLine(exit.p1(), exitOffset);
            utils::Position controlPoint;
            if (!entryLine.intersection(exitLine, controlPoint)) {
                path.addPosition(entryOffset);
                path.addPosition(exitOffset);
                break;
            }
            path.addQuadraticBezier(entryOffset, exitOffset, controlPoint);
            break;
        }
        case topology::MovementGeometryType::CUBIC_BEZIER: {
            const auto& params =
                std::get<topology::CubicBezierParams>(geometrySpec.params);
            utils::Vector2 entryDir = (entry.p2() - entry.p1()).normalized();
            utils::Vector2 exitDir = (exit.p2() - exit.p1()).normalized();
            utils::Position controlPoint1 =
                entryOffset + entryDir * params.control1Distance;
            utils::Position controlPoint2 =
                exitOffset - exitDir * params.control2Distance;
            path.addCubicBezier(entryOffset, exitOffset, controlPoint1,
                                controlPoint2);
            break;
        }
    }
    path.addPosition(exit.p1());
    return path;
}

std::vector<utils::Polyline> calculatePaths(
    Edge fromEdge, std::vector<size_t> entryIndices, Edge toEdge,
    std::vector<size_t> exitIndices,
    const topology::MovementGeometrySpec& geometrySpec) {
    std::vector<utils::Polyline> movementPaths;

    auto leftmostEntry = fromEdge.entries()[entryIndices.front()];
    auto rightmostEntry = fromEdge.entries()[entryIndices.back()];

    auto leftmostExit = toEdge.exits()[exitIndices.front()];
    auto rightmostExit = toEdge.exits()[exitIndices.back()];

    auto entryAnchor = offsetAnchor(leftmostEntry, rightmostEntry);
    auto exitAnchor = offsetAnchor(leftmostExit, rightmostExit);

    auto entryDir = (leftmostEntry.p2() - leftmostEntry.p1()).normalized();
    auto exitDir = (leftmostExit.p2() - leftmostExit.p1()).normalized();

    auto entryOffsetAnchor = entryAnchor + entryDir * geometrySpec.entryOffset;
    auto exitOffsetAnchor = exitAnchor - exitDir * geometrySpec.exitOffset;
    // vector between anchors
    auto bridge = (exitOffsetAnchor - entryOffsetAnchor).normalized();

    // entry mitre direction
    auto entryBisector = (-entryDir + bridge).normalized();

    // exit mitre direction
    auto exitBisector = ((-bridge) + exitDir).normalized();

    DebugSink::drawLine(
        utils::Line(entryOffsetAnchor, entryOffsetAnchor + entryDir), Qt::gray);
    DebugSink::drawLine(
        utils::Line(entryOffsetAnchor, entryOffsetAnchor + bridge),
        Qt::magenta);
    DebugSink::drawLine(
        utils::Line(entryOffsetAnchor, entryOffsetAnchor + entryBisector),
        Qt::blue);

    DebugSink::drawLine(
        utils::Line(exitOffsetAnchor, exitOffsetAnchor + exitDir), Qt::gray);
    DebugSink::drawLine(
        utils::Line(exitOffsetAnchor, exitOffsetAnchor - bridge), Qt::magenta);
    DebugSink::drawLine(
        utils::Line(exitOffsetAnchor, exitOffsetAnchor + exitBisector),
        Qt::blue);

    utils::Line entryOffsetMitreLine(entryOffsetAnchor,
                                     entryOffsetAnchor + entryBisector);
    utils::Line exitOffsetMitreLine(exitOffsetAnchor,
                                    exitOffsetAnchor + exitBisector);

    // Precompute per-entry-lane mitre intersections
    std::vector<utils::Position> entryMitres(entryIndices.size());
    for (size_t i = 0; i < entryIndices.size(); ++i) {
        auto lane = fromEdge.entries()[entryIndices[i]];
        if (!lane.intersection(entryOffsetMitreLine, entryMitres[i]))
            throw std::runtime_error("Mitre intersection failed");
    }

    // Precompute per-exit-lane mitre intersections
    std::vector<utils::Position> exitMitres(exitIndices.size());
    for (size_t j = 0; j < exitIndices.size(); ++j) {
        auto lane = toEdge.exits()[exitIndices[j]];
        if (!lane.intersection(exitOffsetMitreLine, exitMitres[j]))
            throw std::runtime_error("Mitre intersection failed");
    }

    // M paths: exit lane j pairs with entry lane min(j, N-1)
    for (size_t j = 0; j < exitIndices.size(); ++j) {
        size_t i = std::min(j, entryIndices.size() - 1);
        movementPaths.push_back(
            buildPath(fromEdge.entries()[entryIndices[i]], entryMitres[i],
                      exitMitres[j], toEdge.exits()[exitIndices[j]],
                      geometrySpec));
    }

    return movementPaths;
}
}  // namespace

std::unordered_map<MovementId, Movement> MovementFactory::build(
    const Network& network, const topology::MovementStructure& mStructure) {
    std::unordered_map<MovementId, Movement> movements;
    for (const auto& [mId, m] : mStructure.movements()) {
        const topology::EntryLaneGroup& entryGroup =
            network.edge(m.fromEdge()).entry();
        const topology::ExitLaneGroup& exitGroup =
            network.edge(m.toEdge()).exit();

        std::vector<size_t> entryIndices;
        for (const auto& laneId : m.entryLanes())
            entryIndices.push_back(entryGroup.indexOf(laneId));

        std::vector<size_t> exitIndices;
        for (const auto& laneId : m.exitLanes())
            exitIndices.push_back(exitGroup.indexOf(laneId));

        auto paths = calculatePaths(
            geometry::EdgeFactory::build(network, m.fromEdge()), entryIndices,
            geometry::EdgeFactory::build(network, m.toEdge()), exitIndices,
            m.geometrySpec());
        movements.emplace(mId, Movement(std::move(paths)));
    }
    return movements;
}
}  // namespace geometry