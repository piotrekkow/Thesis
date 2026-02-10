#include "geometry/calculators/movement_calculator.h"

#include <qdebug.h>
#include <qnamespace.h>

#include <utility>

#include "debug_sink.h"
#include "geometry/calculators/edge_calculator.h"
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

    DebugSink::drawPoint(p, Qt::red);
    return p;
}

std::vector<utils::Polyline> calculatePaths(
    Edge fromEdge, const topology::LaneRange& laneRange, Edge toEdge,
    const topology::MovementGeometrySpec& geometrySpec) {
    std::vector<utils::Polyline> movementPaths;

    auto leftmostEntry = fromEdge.entries()[laneRange.first()];
    DebugSink::drawLine(leftmostEntry, Qt::red);
    auto rightmostEntry = fromEdge.entries()[laneRange.last()];
    DebugSink::drawLine(rightmostEntry, Qt::red);

    auto leftmostExit = toEdge.exits()[0];
    auto rightmostExit = toEdge.exits()[toEdge.exits().size() - 1];

    auto entryAnchor = offsetAnchor(leftmostEntry, rightmostEntry);
    auto exitAnchor = offsetAnchor(leftmostExit, rightmostExit);

    auto entryDir = (leftmostEntry.p2() - leftmostEntry.p1()).normalized();
    auto exitDir = (leftmostExit.p2() - leftmostExit.p1()).normalized();

    auto entryOffsetAnchor = entryAnchor + entryDir * geometrySpec.entryOffset;
    DebugSink::drawPoint(entryOffsetAnchor, Qt::green);
    auto exitOffsetAnchor = exitAnchor - exitDir * geometrySpec.exitOffset;
    DebugSink::drawPoint(exitOffsetAnchor, Qt::green);

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
    DebugSink::drawPoint(leftmostEntryMitre, Qt::yellow);
    DebugSink::drawPoint(leftmostExitMitre, Qt::red);

    utils::Polyline polyLeft;
    polyLeft.addPosition(leftmostEntry.p1());
    polyLeft.addPosition(leftmostEntryMitre);
    polyLeft.addPosition(leftmostExitMitre);
    polyLeft.addPosition(leftmostExit.p1());
    movementPaths.push_back(polyLeft);

    if (toEdge.exits().size() > 1) {
        utils::Position rightmostEntryMitre;
        if (!rightmostEntry.intersection(entryOffsetMitreLine,
                                         rightmostEntryMitre))
            throw std::runtime_error("Mitre intersection failed");
        utils::Position rightmostExitMitre;
        if (!rightmostExit.intersection(exitOffsetMitreLine,
                                        rightmostExitMitre))
            throw std::runtime_error("Mitre intersection failed");
        utils::Polyline polyRight;
        polyRight.addPosition(rightmostEntry.p1());
        polyRight.addPosition(rightmostEntryMitre);
        polyRight.addPosition(rightmostExitMitre);
        polyRight.addPosition(rightmostExit.p1());
        movementPaths.push_back(polyRight);
    }

    return movementPaths;
}

}  // namespace

std::unordered_map<EdgeId, Movement> MovementCalculator::compute(
    const Network& network, EdgeId fromId, NodeId nodeId) {
    if (!network.node(nodeId).movementStructure().has_value()) {
        std::ostringstream msg;
        msg << "Node " << nodeId
            << " has no movement structure for geometry calculation.";
        throw std::runtime_error(msg.str());
    }

    const auto& movementMap =
        network.node(nodeId).movementStructure().value().movements();

    auto it = movementMap.find(fromId);
    if (it == movementMap.end()) {
        return {};  // Return empty map if no movements for this edge
    }

    const std::vector<topology::Movement>& movements = it->second;

    std::unordered_map<EdgeId, Movement> result;
    for (const auto& m : movements) {
        auto paths = calculatePaths(
            geometry::EdgeCalculator::compute(network, fromId), m.laneRange(),
            geometry::EdgeCalculator::compute(network, m.toEdge()),
            m.geometrySpec());

        result.insert({m.toEdge(), Movement(std::move(paths))});
    }

    return result;
}
}  // namespace geometry