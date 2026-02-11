#include "network.h"

// #include "topology/movement/movement_geometry_spec.h"
#include "topology/movement/movement_geometry_spec.h"
#include "topology/movement/movement_structure.h"
#include "topology/node.h"

Network::Network() {
    auto i1 = createIntersection();
    auto i2 = createIntersection();
    auto i3 = createIntersection();
    auto i4 = createIntersection();

    auto e12 = createTwoWayEdge(i1.second, {20, 10}, i2.second, {100, 50});
    auto e13 = createTwoWayEdge(i1.second, {-20, -20}, i3.second, {-100, -100});
    auto e14 = createTwoWayEdge(i1.second, {20, -20}, i4.second, {100, -100});

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
    auto builder = n1.createMovementBuilder(*this);

    // east
    builder
        .addMovement(
            e12.second, e13.first, {0, 1},
            topology::MovementGeometrySpec::cubicBezier(5.0, 5.0, 8.0, 8.0))
        .addMovement(e12.second, e14.first, {1, 2},
                     topology::MovementGeometrySpec::quadraticBezier())
        .addMovement(
            e12.second, e12.first, {0},
            topology::MovementGeometrySpec::cubicBezier(5.0, 5.0, 8.0, 8.0))
        .addMovement(
            e13.second, e13.first, {0},
            topology::MovementGeometrySpec::cubicBezier(7.0, 4.0, 8.0, 8.0))
        .addMovement(e13.second, e14.first, {1},
                     topology::MovementGeometrySpec::quadraticBezier(5.0, 5.0))
        .addMovement(
            e13.second, e12.first, {2},
            topology::MovementGeometrySpec::cubicBezier(5.0, 5.0, 8.0, 8.0))
        .addMovement(e14.second, e14.first, {0},
                     topology::MovementGeometrySpec::cubicBezier())
        .addMovement(e14.second, e12.first, {1},
                     topology::MovementGeometrySpec::quadraticBezier(1.0, 1.0))
        .addMovement(e14.second, e13.first, {2},
                     topology::MovementGeometrySpec::quadraticBezier(1.0, 1.0));

    n1.setMovementStructure(builder.build());
}

std::pair<IntersectionId, NodeId> Network::createIntersection() {
    auto newIId = intersectionIdGen_.next();
    intersections_.emplace(newIId, topology::Intersection(newIId));
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
    topology::Intersection& intersection = intersections_.at(id.payload());
    return intersection.node(id);
}

const topology::Node& Network::node(NodeId id) const {
    const topology::Intersection& intersection =
        intersections_.at(id.payload());
    return intersection.node(id);
}
