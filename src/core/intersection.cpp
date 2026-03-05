#include "intersection.h"

#include "network.h"

NodeId Intersection::createNode() {
    auto newId = nodeIdGen_.next(id_);
    nodes_.emplace(newId, topology::Node(newId));
    return newId;
}