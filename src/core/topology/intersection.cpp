#include "topology/intersection.h"

namespace topology {

NodeId Intersection::createNode() {
    auto newId = nodeIdGen_.next(id_);
    nodes_.emplace(newId, Node(newId));
    return newId;
}

}  // namespace topology
