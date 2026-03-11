#include "network.h"

// #include "topology/movement/movement_geometry_spec.h"
#include "controller.h"
#include "id.h"
#include "signal_group.h"
#include "topology/edge.h"
#include "topology/lane_group.h"
#include "topology/movement/movement_geometry_spec.h"
#include "topology/movement/movement_structure.h"
#include "topology/node.h"

Network::Network() {
    auto i1 = createIntersection();
    auto i2 = createIntersection();
    auto i3 = createIntersection();
    auto i4 = createIntersection();

    auto e01 = createTwoWayEdge(i1.second, {25, 20}, i2.second, {100, 70});
    auto e23 = createTwoWayEdge(i1.second, {-25, -25}, i3.second, {-100, -100});
    auto e45 = createTwoWayEdge(i1.second, {25, -25}, i4.second, {100, -100});

    auto e0 = e01.first;
    auto e1 = e01.second;
    auto e2 = e23.first;
    auto e3 = e23.second;
    auto e4 = e45.first;
    auto e5 = e45.second;

    edge(e0).entry().createLanes(e0, 1);
    edge(e0).exit().createLanes(e0, 2);
    edge(e1).entry().createLanes(e1, 3);  // from
    edge(e1).exit().createLanes(e1, 2);
    edge(e2).entry().createLanes(e2, 1);
    edge(e2).exit().createLanes(e2, 4);  // to
    edge(e3).entry().createLanes(e3, 3);
    edge(e3).exit().createLanes(e3, 2);
    edge(e4).entry().createLanes(e4, 1);
    edge(e4).exit().createLanes(e4, 2);  // to
    edge(e5).entry().createLanes(e5, 3);
    edge(e5).exit().createLanes(e5, 2);

    // Capture entry LaneGroups to look up LaneIds by index
    const auto& ege1 = edge(e1).entry();
    const auto& ege3 = edge(e3).entry();
    const auto& ege5 = edge(e5).entry();

    auto& n1 = node(i1.second);
    auto builder1 = n1.createMovementBuilder(*this);

    double offset1 = 12.0;
    double coffset = offset1 + 4.0;

    // east
    builder1
        // 0K (edge 1)
        .addMovement(e1, e0, {ege1.laneId(0), ege1.laneId(1)},
                     topology::MovementGeometrySpec::cubicBezier(
                         offset1, offset1, coffset, coffset))
        .addMovement(e1, e2, {ege1.laneId(1), ege1.laneId(2)},
                     topology::MovementGeometrySpec::cubicBezier(
                         offset1, offset1, coffset, coffset))
        .addMovement(
            e1, e4, {ege1.laneId(2)},
            topology::MovementGeometrySpec::quadraticBezier(offset1, offset1))

        // 1K (edge 3)
        .addMovement(e3, e2, {ege3.laneId(0)},
                     topology::MovementGeometrySpec::cubicBezier(
                         offset1, offset1, coffset, coffset))
        .addMovement(e3, e4, {ege3.laneId(1)},
                     topology::MovementGeometrySpec::cubicBezier(
                         offset1, offset1, coffset, coffset))
        .addMovement(
            e3, e0, {ege3.laneId(2)},
            topology::MovementGeometrySpec::quadraticBezier(offset1, offset1))

        // 2K (edge 5)
        .addMovement(e5, e4, {ege5.laneId(0)},
                     topology::MovementGeometrySpec::cubicBezier())
        .addMovement(
            e5, e0, {ege5.laneId(1), ege5.laneId(2)},
            topology::MovementGeometrySpec::quadraticBezier(offset1, offset1));
    // .addMovement(
    //     e5, e2, {ege5.laneId(2)},
    //     topology::MovementGeometrySpec::quadraticBezier(offset1, offset1));

    n1.setMovementStructure(builder1.build());

    // p1 18,15 p2 24.5,6
    auto n1c1 = n1.createCrossing(utils::Position(10, 25),
                                  utils::Position(24.5, 6), 5.0);

    auto& i1c = intersection(i1.first).controller();

    i1c.createSignalGroup<MovementId>(
        SignalGroup::Type::GENERAL_K,
        {n1.movementStructure()->movementsByEdge(e1).at(0),
         n1.movementStructure()->movementsByEdge(e1).at(1),
         n1.movementStructure()->movementsByEdge(e1).at(2)});

    i1c.createSignalGroup<MovementId>(
        SignalGroup::Type::GENERAL_K,
        {n1.movementStructure()->movementsByEdge(e3).at(0),
         n1.movementStructure()->movementsByEdge(e3).at(1),
         n1.movementStructure()->movementsByEdge(e3).at(2)});

    i1c.createSignalGroup<MovementId>(
        SignalGroup::Type::GENERAL_K,
        {n1.movementStructure()->movementsByEdge(e5).at(0),
         n1.movementStructure()->movementsByEdge(e5).at(1)/*,
         n1.movementStructure()->movementsByEdge(e14.second).at(2)*/});

    i1c.createSignalGroup<CrossingId>(SignalGroup::Type::PEDESTRIAN_P, {n1c1});

    i1c.computeIntergreenMatrix(*this);

    // second intersection

    auto i5 = createIntersection();
    auto i6 = createIntersection();
    auto i7 = createIntersection();

    auto e53 = createEdge(i5.second, {-200, 0}, i3.second, {-120, -100});
    auto e36 = createEdge(i3.second, {-120, -120}, i6.second, {-200, -200});
    auto e37 = createEdge(i3.second, {-100, -120}, i7.second, {0, -200});

    edge(e53).entry().createLanes(e53, 1);
    edge(e37).exit().createLanes(e37, 2);
    edge(e36).exit().createLanes(e36, 2);

    const auto& eg13f = edge(e2).entry();
    const auto& eg53 = edge(e53).entry();

    auto& n3 = node(i3.second);
    auto builder3 = n3.createMovementBuilder(*this);
    builder3.addMovement(e2, e36, {eg13f.laneId(0)})
        .addMovement(e53, e37, {eg53.laneId(0)});
    n3.setMovementStructure(builder3.build());
}

std::pair<IntersectionId, NodeId> Network::createIntersection() {
    auto newIId = intersectionIdGen_.next();
    intersections_.emplace(newIId, Intersection(newIId));
    auto newNId = intersection(newIId).createNode();
    return {newIId, newNId};
}

EdgeId Network::createEdge(NodeId from, utils::Position exitPos, NodeId to,
                           utils::Position entryPos) {
    auto newId = edgeIdGen_.next();
    edges_.emplace(newId, topology::Edge(from, exitPos, to, entryPos));

    node(to).addIncomingEdge(newId);
    node(from).addOutgoingEdge(newId);

    return newId;
}

std::pair<EdgeId, EdgeId> Network::createTwoWayEdge(NodeId n1,
                                                    utils::Position p1,
                                                    NodeId n2,
                                                    utils::Position p2) {
    auto e12 = createEdge(n1, p1, n2, p2);
    auto e21 = createEdge(n2, p2, n1, p1);
    return {e12, e21};
}

topology::Node& Network::node(NodeId id) {
    Intersection& intersection = intersections_.at(id.payload());
    return intersection.node(id);
}

const topology::Node& Network::node(NodeId id) const {
    const Intersection& intersection = intersections_.at(id.payload());
    return intersection.node(id);
}
