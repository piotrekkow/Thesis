#pragma once

#include <unordered_map>
#include <vector>

#include "id.h"
#include "topology/movement/lane_range.h"
#include "topology/movement/movement.h"
#include "topology/movement/movement_geometry_spec.h"

class Network;

namespace topology {

class MovementStructure {
   public:
    MovementStructure() = default;
    class Builder {
       public:
        explicit Builder(Network& network, NodeId nodeId);

        // Add a movement from an incoming edge to an outgoing edge using
        // specific lanes Must be called in order: movements for each incoming
        // edge must go from leftmost to rightmost outgoing edge
        Builder& addMovement(
            EdgeId from, EdgeId to, LaneRange laneRange,
            MovementGeometrySpec geometrySpec = MovementGeometrySpec::line());

        MovementStructure build();

       private:
        Network& network_;
        NodeId nodeId_;
        std::unordered_map<EdgeId, std::vector<Movement>> movements_;

        const std::vector<EdgeId>& incomingEdges() const;
        const std::vector<EdgeId>& outgoingEdges() const;
    };

    const std::unordered_map<EdgeId, std::vector<Movement>>& movements() const {
        return movements_;
    }

   private:
    friend class Builder;
    explicit MovementStructure(
        std::unordered_map<EdgeId, std::vector<Movement>> movementStructure);

    std::unordered_map<EdgeId, std::vector<Movement>> movements_;
};

}  // namespace topology