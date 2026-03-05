#include "network.h"

// #include "topology/movement/movement_geometry_spec.h"
#include "controller.h"
#include "id.h"
#include "signal_group.h"
#include "topology/movement/movement_geometry_spec.h"
#include "topology/movement/movement_structure.h"
#include "topology/node.h"

Network::Network() {
    auto i1 = createIntersection();
    auto i2 = createIntersection();
    auto i3 = createIntersection();
    auto i4 = createIntersection();

    auto e12 = createTwoWayEdge(i1.second, {25, 20}, i2.second, {100, 70});
    auto e13 = createTwoWayEdge(i1.second, {-25, -25}, i3.second, {-100, -100});
    auto e14 = createTwoWayEdge(i1.second, {25, -25}, i4.second, {100, -100});

    edge(e12.first).entry().createLanes(1);
    edge(e12.first).exit().createLanes(2);
    edge(e12.second).entry().createLanes(3);  // from
    edge(e12.second).exit().createLanes(2);
    edge(e13.first).entry().createLanes(1);
    edge(e13.first).exit().createLanes(4);  // to
    edge(e13.second).entry().createLanes(3);
    edge(e13.second).exit().createLanes(2);
    edge(e14.first).entry().createLanes(1);
    edge(e14.first).exit().createLanes(2);  // to
    edge(e14.second).entry().createLanes(3);
    edge(e14.second).exit().createLanes(2);

    auto& n1 = node(i1.second);
    auto builder1 = n1.createMovementBuilder(*this);

    double offset1 = 12.0;
    double coffset = offset1 + 4.0;

    // east
    builder1
        .addMovement(e12.second, e13.first, {0, 1},
                     topology::MovementGeometrySpec::cubicBezier(
                         offset1, offset1, coffset, coffset))
        .addMovement(
            e12.second, e14.first, {1, 2},
            topology::MovementGeometrySpec::quadraticBezier(offset1, offset1))
        .addMovement(e12.second, e12.first, {0},
                     topology::MovementGeometrySpec::cubicBezier(
                         offset1, offset1, coffset, coffset))
        .addMovement(e13.second, e13.first, {0},
                     topology::MovementGeometrySpec::cubicBezier(
                         offset1, offset1, coffset, coffset))
        .addMovement(
            e13.second, e14.first, {1},
            topology::MovementGeometrySpec::quadraticBezier(offset1, offset1))
        .addMovement(e13.second, e12.first, {2},
                     topology::MovementGeometrySpec::cubicBezier(
                         offset1, offset1, coffset, coffset))
        .addMovement(e14.second, e14.first, {0},
                     topology::MovementGeometrySpec::cubicBezier())
        .addMovement(
            e14.second, e12.first, {1},
            topology::MovementGeometrySpec::quadraticBezier(offset1, offset1))
        .addMovement(
            e14.second, e13.first, {2},
            topology::MovementGeometrySpec::quadraticBezier(offset1, offset1));

    n1.setMovementStructure(builder1.build());

    // p1 18,15 p2 24.5,6
    auto n1c1 = n1.createCrossing(utils::Position(10, 25),
                                  utils::Position(24.5, 6), 5.0);

    auto& i1c = intersection(i1.first).controller();

    i1c.createSignalGroup<MovementId>(
        SignalGroup::Type::GENERAL_K,
        {n1.movementStructure()->movementsByEdge(e12.second).at(0),
         n1.movementStructure()->movementsByEdge(e12.second).at(1),
         n1.movementStructure()->movementsByEdge(e12.second).at(2)});

    i1c.createSignalGroup<MovementId>(
        SignalGroup::Type::GENERAL_K,
        {n1.movementStructure()->movementsByEdge(e13.second).at(0),
         n1.movementStructure()->movementsByEdge(e13.second).at(1),
         n1.movementStructure()->movementsByEdge(e13.second).at(2)});

    i1c.createSignalGroup<MovementId>(
        SignalGroup::Type::GENERAL_K,
        {n1.movementStructure()->movementsByEdge(e14.second).at(0),
         n1.movementStructure()->movementsByEdge(e14.second).at(1),
         n1.movementStructure()->movementsByEdge(e14.second).at(2)});

    i1c.createSignalGroup<CrossingId>(SignalGroup::Type::PEDESTRIAN_P, {n1c1});

    i1c.computeIntergreenMatrix(*this);

    // second intersection

    auto i5 = createIntersection();
    auto i6 = createIntersection();
    auto i7 = createIntersection();

    auto e53 = createEdge(i5.second, {-200, 0}, i3.second, {-120, -100});
    auto e36 = createEdge(i3.second, {-120, -120}, i6.second, {-200, -200});
    auto e37 = createEdge(i3.second, {-100, -120}, i7.second, {0, -200});

    edge(e53).entry().createLanes(1);
    edge(e37).exit().createLanes(2);
    edge(e36).exit().createLanes(2);

    auto& n3 = node(i3.second);
    auto builder3 = n3.createMovementBuilder(*this);
    builder3.addMovement(e13.first, e36, {0}).addMovement(e53, e37, {0});
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
