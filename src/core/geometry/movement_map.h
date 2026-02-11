#pragma once

#include <unordered_map>

#include "geometry/movement.h"
#include "id.h"

class Network;

namespace geometry {
class MovementMap {
   public:
    static MovementMap build(const Network& network, NodeId nodeId);
    const std::unordered_map<EdgeId, Movement>* tryFrom(EdgeId from) const;
    const Movement* tryFind(EdgeId from, EdgeId to) const;

    const std::unordered_map<EdgeId, std::unordered_map<EdgeId, Movement>>&
    movementMap() const {
        return movementMap_;
    }

   private:
    // map of key: fromId (entering) edges and value: (map of key: toId
    // (exiting) edges and value: movement geometry)
    std::unordered_map<EdgeId, std::unordered_map<EdgeId, Movement>>
        movementMap_;
};
}  // namespace geometry