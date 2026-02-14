#pragma once

#include <unordered_map>
#include <vector>

#include "id.h"
#include "id_generator.h"
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
        ScopedIdGenerator<MovementTag, NodeId> movementIdGen_;
        std::unordered_map<MovementId, Movement> movements_;
        std::unordered_map<EdgeId, std::vector<MovementId>> movementsByEdge_;

        const std::vector<EdgeId>& incomingEdges() const;
        const std::vector<EdgeId>& outgoingEdges() const;

        bool validLaneSharing(const std::vector<MovementId>& ids,
                              LaneRange laneRange);
        bool validLaneUtilization(const std::vector<MovementId>& movements,
                                  size_t laneCount);
    };

    const std::unordered_map<MovementId, Movement>& movements() const {
        return movements_;
    }

    const Movement& movement(const MovementId id) const {
        return movements_.at(id);
    }
    const std::vector<MovementId>& movementsByEdge(EdgeId id) const {
        return movementsByEdge_.at(id);
    }

    const std::unordered_map<EdgeId, std::vector<MovementId>>&
    movementsAllEdges() const {
        return movementsByEdge_;
    }

   private:
    friend class Builder;
    explicit MovementStructure(
        std::unordered_map<MovementId, Movement> movements,
        std::unordered_map<EdgeId, std::vector<MovementId>> movementsByEdge);

    std::unordered_map<MovementId, Movement> movements_;
    std::unordered_map<EdgeId, std::vector<MovementId>> movementsByEdge_;
};

}  // namespace topology