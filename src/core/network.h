#pragma once

#include <unordered_map>
#include <utility>

#include "id.h"
#include "id_generator.h"
#include "intersection.h"
#include "position.h"
#include "topology/edge.h"


namespace topology {
class Node;
}

class Network {
   public:
    Network();
    ~Network() = default;

    std::pair<IntersectionId, NodeId> createIntersection();
    EdgeId createEdge(NodeId from, utils::Position exitPos, NodeId to,
                      utils::Position entryPos);
    std::pair<EdgeId, EdgeId> createTwoWayEdge(NodeId n1, utils::Position p1,
                                               NodeId n2, utils::Position p2);

    topology::Edge& edge(EdgeId id) { return edges_.at(id); }
    Intersection& intersection(IntersectionId id) {
        return intersections_.at(id);
    }
    topology::Node& node(NodeId id);

    const topology::Edge& edge(EdgeId id) const { return edges_.at(id); }
    const Intersection& intersection(IntersectionId id) const {
        return intersections_.at(id);
    }
    const topology::Node& node(NodeId id) const;

    const std::unordered_map<EdgeId, topology::Edge>& edges() const {
        return edges_;
    }
    const std::unordered_map<IntersectionId, Intersection>& intersections()
        const {
        return intersections_;
    }

   private:
    std::unordered_map<EdgeId, topology::Edge> edges_;
    std::unordered_map<IntersectionId, Intersection> intersections_;

    GlobalIdGenerator<EdgeTag> edgeIdGen_;
    GlobalIdGenerator<IntersectionTag> intersectionIdGen_;
};