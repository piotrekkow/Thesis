#include "network.h"

#include "edge.h"
#include "intersection.h"
#include "node.h"
#include "position.h"


Network::Network() {
    auto i1 = createIntersection();
    auto i2 = createIntersection();
    auto i3 = createIntersection();
    auto i4 = createIntersection();

    auto e12 = createTwoWayEdge(i1.second, {10, 5}, i2.second, {100, 50});
    auto e13 = createTwoWayEdge(i1.second, {-10, -10}, i3.second, {-100, -100});
    auto e14 = createTwoWayEdge(i1.second, {10, -10}, i4.second, {100, -100});
}

std::pair<IntersectionId, NodeId> Network::createIntersection() {
    auto newIId = intersectionIdGen_.next();
    intersections_.emplace(newIId, Intersection(newIId));
    auto newNId = intersection(newIId).createNode();
    return {newIId, newNId};
}

EdgeId Network::createEdge(NodeId from, geom::Position entryPos, NodeId to,
                           geom::Position exitPos) {
    auto newId = edgeIdGen_.next();
    edges_.emplace(newId, Edge(from, entryPos, to, exitPos));

    node(to).addIncomingEdge(newId);
    node(from).addOutgoingEdge(newId);

    return newId;
}

std::pair<EdgeId, EdgeId> Network::createTwoWayEdge(NodeId n1,
                                                    geom::Position p1,
                                                    NodeId n2,
                                                    geom::Position p2) {
    auto e12 = createEdge(n1, p1, n2, p2);
    auto e21 = createEdge(n2, p2, n1, p1);
    return {e12, e21};
}

Node& Network::node(NodeId id) {
    Intersection& intersection = intersections_.at(id.payload());
    return intersection.node(id);
}
