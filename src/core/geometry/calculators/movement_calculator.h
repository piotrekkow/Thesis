#pragma once

#include <unordered_map>

#include "geometry/movement.h"
#include "id.h"

class Network;

namespace geometry {

class MovementCalculator {
   public:
    // @return computed for fromId edge map of key: toId (exiting) edges and
    // value: movement geometry
    static std::unordered_map<EdgeId, Movement> compute(const Network& network,
                                                        EdgeId fromId,
                                                        NodeId nodeId);
};

}  // namespace geometry