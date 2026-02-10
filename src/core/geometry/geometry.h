#pragma once

#include <unordered_map>

#include "geometry/edge.h"
#include "geometry/node.h"
#include "id.h"

class Network;

namespace geometry {

class Geometry {
   public:
    explicit Geometry(const Network& network);

    void calculateEdge(EdgeId id);
    void calculateNode(NodeId id);

    const Edge& edgeGeometry(EdgeId id) const { return edgeGeometries_.at(id); }
    const Node& nodeGeometry(NodeId id) const { return nodeGeometries_.at(id); }

   private:
    const Network& network_;
    std::unordered_map<EdgeId, Edge> edgeGeometries_;
    std::unordered_map<NodeId, Node> nodeGeometries_;
};

}  // namespace geometry
