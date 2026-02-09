#pragma once

#include "id.h"
#include "lane_range.h"
#include "movement_geometry_spec.h"

class Movement {
   public:
    Movement(EdgeId fromEdge, LaneRange laneRange, EdgeId toEdge,
             double heading,
             MovementGeometrySpec geometrySpec = MovementGeometrySpec::line())
        : fromEdge_(fromEdge),
          laneRange_(laneRange),
          toEdge_(toEdge),
          heading_(heading),
          geometrySpec_(geometrySpec) {}

    EdgeId fromEdge() const { return fromEdge_; }
    const LaneRange& laneRange() const { return laneRange_; }
    EdgeId toEdge() const { return toEdge_; }
    double heading() const { return heading_; }
    MovementGeometrySpec& geometrySpec() { return geometrySpec_; }

   private:
    EdgeId fromEdge_;
    LaneRange laneRange_;
    EdgeId toEdge_;
    double heading_;
    MovementGeometrySpec geometrySpec_;
};