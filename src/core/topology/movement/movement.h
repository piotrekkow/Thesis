#pragma once

#include <vector>

#include "id.h"
#include "topology/movement/movement_geometry_spec.h"

namespace topology {

class Movement {
   public:
    Movement(EdgeId fromEdge, std::vector<EntryLaneId> entryLanes, EdgeId toEdge,
             std::vector<ExitLaneId> exitLanes,
             MovementGeometrySpec geometrySpec = MovementGeometrySpec::line())
        : fromEdge_(fromEdge),
          entryLanes_(std::move(entryLanes)),
          toEdge_(toEdge),
          exitLanes_(std::move(exitLanes)),
          geometrySpec_(geometrySpec) {}

    EdgeId fromEdge() const { return fromEdge_; }
    const std::vector<EntryLaneId>& entryLanes() const { return entryLanes_; }
    EdgeId toEdge() const { return toEdge_; }
    const std::vector<ExitLaneId>& exitLanes() const { return exitLanes_; }
    const MovementGeometrySpec& geometrySpec() const { return geometrySpec_; }

   private:
    EdgeId fromEdge_;
    std::vector<EntryLaneId> entryLanes_;
    EdgeId toEdge_;
    std::vector<ExitLaneId> exitLanes_;
    MovementGeometrySpec geometrySpec_;
};

}  // namespace topology
