#pragma once

#include <unordered_map>

#include "id.h"
#include "id_generator.h"
#include "node.h"

namespace geom {
class Position;
}

class Intersection {
   public:
    explicit Intersection(IntersectionId id) : id_(id) {}
    ~Intersection() = default;

    Intersection(const Intersection&) = delete;
    Intersection& operator=(const Intersection&) = delete;

    Intersection(Intersection&&) = default;
    Intersection& operator=(Intersection&&) = default;

    NodeId createNode();

    Node& node(NodeId id) { return nodes_.at(id); }
    const std::unordered_map<NodeId, Node>& nodes() const { return nodes_; }

   private:
    IntersectionId id_;  // used only to create a scope for generated node ids
    std::unordered_map<NodeId, Node> nodes_;
    ScopedIdGenerator<NodeTag, IntersectionId> nodeIdGen_;
};