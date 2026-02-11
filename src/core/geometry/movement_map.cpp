#include "geometry/movement_map.h"

#include "geometry/calculators/movement_calculator.h"
#include "network.h"
#include "topology/node.h"

namespace geometry {

MovementMap MovementMap::build(const Network& network, NodeId nodeId) {
    MovementMap mm;
    const auto& node = network.node(nodeId);
    for (const auto& from : node.incomingEdges()) {
        auto movements = MovementCalculator::compute(network, from, nodeId);
        if (!movements.empty()) {
            mm.movementMap_.emplace(from, std::move(movements));
        }
    }
    return mm;
}

// may not exist, therefore returns a pointer
const std::unordered_map<EdgeId, Movement>* MovementMap::tryFrom(
    EdgeId from) const {
    auto it = movementMap_.find(from);
    if (it == movementMap_.end()) return nullptr;
    return &it->second;
}

// may not exist, therefore returns a pointer
const Movement* MovementMap::tryFind(EdgeId from, EdgeId to) const {
    auto outer = movementMap_.find(from);
    if (outer == movementMap_.end()) return nullptr;
    auto inner = outer->second.find(to);
    if (inner == outer->second.end()) return nullptr;
    return &inner->second;
}

}  // namespace geometry
