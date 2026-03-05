#pragma once
#include <unordered_map>

#include "geometry/movement.h"
#include "id.h"

class Network;
namespace topology {
class MovementStructure;
}

namespace geometry {

class MovementFactory {
   public:
    static std::unordered_map<MovementId, Movement> build(
        const Network& network, const topology::MovementStructure& mStructure);
};

}  // namespace geometry