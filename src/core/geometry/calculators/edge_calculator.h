#pragma once

#include "geometry/edge.h"
#include "id.h"

class Network;

namespace geometry {

class EdgeCalculator {
   public:
    static Edge compute(const Network& network, EdgeId id);

   private:
    static std::vector<utils::Position> lanePositions(
        utils::Position handle, double heading, std::vector<double> widths);
};

}  // namespace geometry
