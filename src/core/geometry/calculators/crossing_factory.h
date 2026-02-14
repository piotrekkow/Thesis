#pragma once

#include "geometry/crossing.h"
#include "id.h"

class Network;

namespace geometry {

class CrossingFactory {
   public:
    static Crossing build(const Network& network, CrossingId id);
};

}  // namespace geometry