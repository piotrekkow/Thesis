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
#include "topology/movement/lane_range.h"
#include "topology/movement/movement.h"
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
    Edge fromEdge, const topology::LaneRange& laneRange, Edge toEdge,
    const topology::MovementGeometrySpec& geometrySpec) {
    std::vector<utils::Polyline> movementPaths;

    auto leftmostEntry = fromEdge.entries()[laneRange.first()];
    auto rightmostEntry = fromEdge.entries()[laneRange.last()];

    auto leftmostExit = toEdge.exits()[0];
    auto rightmostExit = toEdge.exits()[toEdge.exits().size() - 1];

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

    utils::Position leftmostEntryMitre;
    if (!leftmostEntry.intersection(entryOffsetMitreLine, leftmostEntryMitre))
        throw std::runtime_error("Mitre intersection failed");
    utils::Position leftmostExitMitre;
    if (!leftmostExit.intersection(exitOffsetMitreLine, leftmostExitMitre))
        throw std::runtime_error("Mitre intersection failed");

    movementPaths.push_back(buildPath(leftmostEntry, leftmostEntryMitre,
                                      leftmostExitMitre, leftmostExit,
                                      geometrySpec));

    if (toEdge.exits().size() > 1) {
        utils::Position rightmostEntryMitre;
        if (!rightmostEntry.intersection(entryOffsetMitreLine,
                                         rightmostEntryMitre))
            throw std::runtime_error("Mitre intersection failed");
        utils::Position rightmostExitMitre;
        if (!rightmostExit.intersection(exitOffsetMitreLine,
                                        rightmostExitMitre))
            throw std::runtime_error("Mitre intersection failed");
        movementPaths.push_back(buildPath(rightmostEntry, rightmostEntryMitre,
                                          rightmostExitMitre, rightmostExit,
                                          geometrySpec));
    }

    return movementPaths;
}
}  // namespace

// std::unordered_map<EdgeId, Movement> MovementCalculator::compute(
//     const Network& network, EdgeId fromId, NodeId nodeId) {
//     if (!network.node(nodeId).movementStructure().has_value()) {
//         std::ostringstream msg;
//         msg << "Node " << nodeId
//             << " has no movement structure for geometry calculation.";
//         throw std::runtime_error(msg.str());
//     }

//     const auto& movementMap =
//         network.node(nodeId).movementStructure().value().movements();

//     auto it = movementMap.find(fromId);
//     if (it == movementMap.end()) {
//         return {};  // Return empty map if no movements for this edge
//     }

//     const std::vector<topology::Movement>& movements = it->second;

//     std::unordered_map<EdgeId, Movement> result;
//     for (const auto& m : movements) {
//         auto paths = calculatePaths(
//             geometry::EdgeFactory::build(network, fromId), m.laneRange(),
//             geometry::EdgeFactory::build(network, m.toEdge()),
//             m.geometrySpec());

//         result.insert({m.toEdge(), Movement(std::move(paths))});
//     }

//     return result;
// }

MovementMap MovementFactory::build(
    const Network& network, const topology::MovementStructure& mStructure) {
    std::unordered_map<MovementId, Movement> movements;
    for (const auto& [mId, m] : mStructure.movements()) {
        auto paths = calculatePaths(
            geometry::EdgeFactory::build(network, m.fromEdge()), m.laneRange(),
            geometry::EdgeFactory::build(network, m.toEdge()),
            m.geometrySpec());
        movements.emplace(mId, Movement(std::move(paths)));
    }
    return MovementMap(std::move(movements));
}
}  // namespace geometry