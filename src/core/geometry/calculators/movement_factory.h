#pragma once

#include "geometry/movement_map.h"

class Network;
namespace topology {
class MovementStructure;
}

namespace geometry {

class MovementFactory {
   public:
    static MovementMap build(const Network& network,
                             const topology::MovementStructure& mStructure);
};

}  // namespace geometry