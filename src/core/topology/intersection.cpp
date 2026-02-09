#include "intersection.h"

#include "node.h"

NodeId Intersection::createNode() {
    auto newId = nodeIdGen_.next(id_);
    nodes_.emplace(newId, Node(newId));
    return newId;
}
