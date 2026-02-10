#pragma once

#include <unordered_map>

#include "geometry/movement.h"
#include "id.h"

class Network;

namespace geometry {

class MovementCalculator {
   public:
    // @return movement geometry for a given exit edge
    static std::unordered_map<EdgeId, Movement> compute(const Network& network,
                                                        EdgeId fromId,
                                                        NodeId nodeId);
};

}  // namespace geometry