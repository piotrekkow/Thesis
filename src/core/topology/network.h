#pragma once

#include <unordered_map>
#include <utility>

#include "edge.h"
#include "id.h"
#include "id_generator.h"
#include "intersection.h"
#include "position.h"

class Node;

class Network {
   public:
    Network();
    ~Network() = default;

    std::pair<IntersectionId, NodeId> createIntersection();
    EdgeId createEdge(NodeId from, geom::Position entryPos, NodeId to,
                      geom::Position exitPos);
    std::pair<EdgeId, EdgeId> createTwoWayEdge(NodeId n1, geom::Position p1,
                                               NodeId n2, geom::Position p2);

    Edge& edge(EdgeId id) { return edges_.at(id); }
    Intersection& intersection(IntersectionId id) {
        return intersections_.at(id);
    }
    Node& node(NodeId id);

    const std::unordered_map<EdgeId, Edge>& edges() const { return edges_; }
    const std::unordered_map<IntersectionId, Intersection>& intersections()
        const {
        return intersections_;
    }

   private:
    std::unordered_map<EdgeId, Edge> edges_;
    std::unordered_map<IntersectionId, Intersection> intersections_;

    GlobalIdGenerator<EdgeTag> edgeIdGen_;
    GlobalIdGenerator<IntersectionTag> intersectionIdGen_;
};